extern crate itertools;
extern crate phf;
extern crate proc_macro;
extern crate quote;
extern crate syn;

mod parse_latex;

use itertools::Itertools;
use proc_macro::TokenStream;
use quote::quote;
use syn::parse::Parser;
use syn::{Token, Lit, Expr};
use syn::punctuated::Punctuated;

use parse_latex::{parse_latex, LatexToken, MathCommand};


macro_rules! try_match {
    ($enum_value:path[ $var:expr ]) => {
        match $var {
            $enum_value(v) => Some(v),
            _ => None,
        }
    };
}

fn make_format_for_token(token: LatexToken) -> String {
    match token {
        LatexToken::Placeholder() => String::from("{}"),
        LatexToken::Literal(ch) => String::from(ch),
        LatexToken::Command(cmd) => {
            use MathCommand::*;
            // TODO: Consider a more elegant/safe way of extracting args with something like
            //     format_with_args!("{} / {}", 2 args)
            //   which internally does roughly:
            //     assert_eq!(2, num_command_args(cmd))
            //     format!("{} / {}", args.0, args.1)
            // TODO: Fix sub- and super-script (need to pass information about context for
            //   encoders other than LaTeX)
            let args = cmd.args.into_iter().map(|a| make_format_for_token(a)).collect_vec();
            match cmd.command {
                Subscript => format!("_{}", args[0]),
                Superscript => format!("^{}", args[0]),
                Fraction => format!("{} / {}", args[0], args[1]),
                Space => String::from(" "),
                Infinity => String::from("∞"),
            }
        }
        LatexToken::Sequence(seq) => seq.into_iter().map(|a| make_format_for_token(a)).join("")
    }
}

fn make_format_string(src: &str) -> String {
    make_format_for_token(parse_latex(src))
}

#[proc_macro]
pub fn math_format(input: TokenStream) -> TokenStream {
    let format_err = "The first argument should specify format as literal string";
    let args = Punctuated::<Expr, Token![,]>::parse_separated_nonempty.parse(input).unwrap();
    let mut args_iter = args.into_iter();
    let format_expr = args_iter.next().expect(format_err);
    let format_lit = try_match!(Expr::Lit[format_expr]).expect(format_err);
    let format_lit_str = try_match!(Lit::Str[format_lit.lit]).expect(format_err);
    let format_string = make_format_string(&format_lit_str.value());
    quote! {
        format!(#format_string, #(#args_iter),*)
    }.into()
}
