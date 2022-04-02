extern crate itertools;
extern crate phf;
extern crate proc_macro;
extern crate proc_macro2;
extern crate quote;
extern crate syn;

mod parse_latex;

use itertools::Itertools;
use proc_macro::TokenStream;
use quote::{quote, quote_spanned, ToTokens};
use syn::{Token, Lit, Expr};
use syn::parse::Parser;
use syn::punctuated::Punctuated;
use syn::spanned::Spanned;


macro_rules! try_match {
    ($enum_value:path[ $var:expr ]) => {
        match $var {
            $enum_value(ref v) => Some(v),
            _ => None,
        }
    };
}

fn make_format_for_token(token: parse_latex::Token, macro_args: &mut Vec<Option<Expr>>) -> Expr {
    use parse_latex::Token;
    use parse_latex::PlaceholderFormat;
    syn::Expr::Verbatim(match token {
        Token::Placeholder(placeholder) => {
            let arg = macro_args[placeholder.arg_index].take().unwrap();
            match placeholder.format {
                PlaceholderFormat::Atom => quote! { math_format::mfmt::lit(#arg) },
                PlaceholderFormat::List => quote! { math_format::mfmt::comma_list(#arg) },
            }
        }
        Token::Literal(ch) => {
            // TODO: Group literals together
            quote! { math_format::mfmt::lit(#ch.to_string()) }
        }
        Token::Command(cmd) => {
            use parse_latex::MathCommand as MC;
            let args = cmd.args.into_iter().map(|a| make_format_for_token(a, macro_args));
            match cmd.command {
                MC::Subscript => quote!{ math_format::mfmt::sub(#(#args),*) },
                MC::Superscript => quote!{ math_format::mfmt::sup(#(#args),*) },
                MC::OperatorName => quote!{ math_format::mfmt::op(#(#args),*) },
                MC::Fraction => quote!{ math_format::mfmt::frac(#(#args),*) },

                MC::Space => quote!{ math_format::mfmt::space(#(#args),*) },
                MC::Infinity => quote!{ math_format::mfmt::inf(#(#args),*) },

                MC::Red => quote!{ math_format::mfmt::color(math_format::Color::Red, #(#args),*) },
                MC::Green => quote!{ math_format::mfmt::color(math_format::Color::Green, #(#args),*) },
                MC::Yellow => quote!{ math_format::mfmt::color(math_format::Color::Yellow, #(#args),*) },
                MC::Blue => quote!{ math_format::mfmt::color(math_format::Color::Blue, #(#args),*) },
                MC::Magenta => quote!{ math_format::mfmt::color(math_format::Color::Magenta, #(#args),*) },
                MC::Cyan => quote!{ math_format::mfmt::color(math_format::Color::Cyan, #(#args),*) },
                MC::Orange => quote!{ math_format::mfmt::color(math_format::Color::Orange, #(#args),*) },
                MC::Gray => quote!{ math_format::mfmt::color(math_format::Color::Gray, #(#args),*) },
                MC::PaleRed => quote!{ math_format::mfmt::color(math_format::Color::PaleRed, #(#args),*) },
                MC::PaleGreen => quote!{ math_format::mfmt::color(math_format::Color::PaleGreen, #(#args),*) },
                MC::PaleBlue => quote!{ math_format::mfmt::color(math_format::Color::PaleBlue, #(#args),*) },
                MC::PaleMagenta => quote!{ math_format::mfmt::color(math_format::Color::PaleMagenta, #(#args),*) },
                MC::PaleCyan => quote!{ math_format::mfmt::color(math_format::Color::PaleCyan, #(#args),*) },
            }
        }
        Token::Sequence(seq) => {
            let args = seq.into_iter().map(|a| make_format_for_token(a, macro_args));
            quote! { math_format::mfmt::concat(vec![#(#args),*]) }
        }
    })
}

fn make_format_expr(src: &str, macro_args: &mut Vec<Option<Expr>>) -> Result<Expr, String> {
    let latex = parse_latex::parse_document(src).map_err(|err| err.message)?;
    if latex.num_placeholders != macro_args.len() {
        return Err(format!("Expected {} args, but got {}", latex.num_placeholders, macro_args.len()));
    }
    Ok(make_format_for_token(latex.root, macro_args))
}

#[proc_macro]
pub fn math_format_node(input: TokenStream) -> TokenStream {
    let format_err = "The first argument should specify format as literal string";
    let args = Punctuated::<Expr, Token![,]>::parse_terminated.parse(input).unwrap();
    let mut arg_iter = args.into_iter();
    let format_expr = arg_iter.next().expect(format_err);
    let format_lit = try_match!(Expr::Lit[format_expr]).expect(format_err);
    let format_lit_str = try_match!(Lit::Str[format_lit.lit]).expect(format_err);
    let mut macro_args = arg_iter.map(|a| Some(a)).collect_vec();
    match make_format_expr(&format_lit_str.value(), &mut macro_args) {
        Err(err) => {
            quote_spanned!(format_expr.span()=> compile_error!(#err)).into()
        }
        Ok(expr) => {
            assert!(macro_args.iter().all(|a| a.is_none()));
            expr.to_token_stream().into()
        }
    }
}

#[proc_macro]
pub fn math_format(input: TokenStream) -> TokenStream {
    let format_node: proc_macro2::TokenStream = math_format_node(input).into();
    quote!{
        #format_node .render()
    }.into()
}
