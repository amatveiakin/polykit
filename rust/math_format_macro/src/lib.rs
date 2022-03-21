extern crate itertools;
extern crate phf;
extern crate proc_macro;
extern crate proc_macro2;
extern crate quote;
extern crate syn;

mod parse_latex;

use itertools::Itertools;
use proc_macro::TokenStream;
use quote::{quote, ToTokens};
use syn::{Token, Lit, Expr};
use syn::parse::Parser;
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

fn make_format_for_token(token: LatexToken, macro_args: &mut Vec<Option<Expr>>) -> Expr {
    syn::Expr::Verbatim(match token {
        LatexToken::Placeholder(index) => {
            let arg = macro_args[index].take().unwrap();
            // TODO: Support things other than literals (like comma-separated lists)
            quote! { math_format::mfmt::lit((#arg).to_string()) }
        }
        LatexToken::Literal(ch) => {
            quote! { math_format::mfmt::lit(#ch.to_string()) }
        }
        LatexToken::Command(cmd) => {
            use MathCommand as MC;
            let args = cmd.args.into_iter().map(|a| make_format_for_token(a, macro_args));
            match cmd.command {
                MC::Subscript => quote!{ math_format::mfmt::sub(#(#args),*) },
                MC::Superscript => quote!{ math_format::mfmt::sup(#(#args),*) },
                MC::Fraction => quote!{ math_format::mfmt::frac(#(#args),*) },
                MC::Space => quote!{ math_format::mfmt::space(#(#args),*) },
                MC::Infinity => quote!{ math_format::mfmt::inf(#(#args),*) },
            }
        }
        LatexToken::Sequence(seq) => {
            let args = seq.into_iter().map(|a| make_format_for_token(a, macro_args));
            quote! { math_format::mfmt::concat(vec![#(#args),*]) }
        }
    })
}

fn make_format_expr(src: &str, macro_args: &mut Vec<Option<Expr>>) -> Expr {
    let latex = parse_latex(src);
    assert!(
        latex.num_placeholders == macro_args.len(),
        "Expected {} args, but got {}", latex.num_placeholders, macro_args.len()
    );
    make_format_for_token(latex.root, macro_args)
}

#[proc_macro]
pub fn math_format(input: TokenStream) -> TokenStream {
    let format_err = "The first argument should specify format as literal string";
    let args = Punctuated::<Expr, Token![,]>::parse_separated_nonempty.parse(input).unwrap();
    let mut arg_iter = args.into_iter();
    let format_expr = arg_iter.next().expect(format_err);
    let format_lit = try_match!(Expr::Lit[format_expr]).expect(format_err);
    let format_lit_str = try_match!(Lit::Str[format_lit.lit]).expect(format_err);
    let mut macro_args = arg_iter.map(|a| Some(a)).collect_vec();
    let format_expr = make_format_expr(&format_lit_str.value(), &mut macro_args);
    assert!(macro_args.iter().all(|a| a.is_none()));
    format_expr.to_token_stream().into()
}
