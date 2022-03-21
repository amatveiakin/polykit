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

use parse_latex::{parse_latex, LatexToken, MathCommand, num_command_args};


macro_rules! try_match {
    ($enum_value:path[ $var:expr ]) => {
        match $var {
            $enum_value(v) => Some(v),
            _ => None,
        }
    };
}

macro_rules! format_tuple {
    ($format_str:literal 0 $args:ident) => { format!($format_str) };
    ($format_str:literal 1 $args:ident) => { format!($format_str, $args[0]) };
    ($format_str:literal 2 $args:ident) => { format!($format_str, $args[0], $args[1]) };
}

macro_rules! format_command {
    ($cmd:ident with $num_args:tt $args:ident => $format_str:literal) => {{
        assert_eq!($num_args, num_command_args($cmd.command));
        format_tuple!($format_str $num_args $args)
    }};
}

fn make_format_for_token(token: LatexToken) -> String {
    match token {
        LatexToken::Placeholder() => String::from("{}"),
        LatexToken::Literal(ch) => String::from(ch),
        LatexToken::Command(cmd) => {
            use MathCommand::*;
            // TODO: Fix sub- and super-script (need to pass information about context for
            //   encoders other than LaTeX)
            let args = cmd.args.into_iter().map(|a| make_format_for_token(a)).collect_vec();
            match cmd.command {
                Subscript => format_command!(cmd with 1 args => "_{}"),
                Superscript => format_command!(cmd with 1 args => "^{}"),
                Fraction => format_command!(cmd with 2 args => "{} / {}"),
                Space => format_command!(cmd with 0 args => " "),
                Infinity => format_command!(cmd with 0 args => "∞"),
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
