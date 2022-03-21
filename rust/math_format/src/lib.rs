extern crate derive_new;
extern crate itertools;

mod unicode_alphabets;

use std::fmt;

use derive_new::new;
use itertools::Itertools;

use unicode_alphabets::{unicode_to_subscript, unicode_to_superscript};


#[derive(PartialEq, Eq, Clone, Copy, Debug)]
pub enum VPos {
    Normal,
    Sub,
    Super,
}

#[derive(PartialEq, Eq, Clone, Copy, Debug)]
pub enum FontStyle {
    MathNormal,
    MathStraight,
}

pub enum FormatNode {
    Literal(Literal),
    SpecialCharacter(SpecialCharacter),
    Concatenation(Concatenation),
    CommaSeparatedList(CommaSeparatedList),
    OperatorName(OperatorName),
    Fraction(Fraction),
    Subscript(Subscript),
    Superscript(Superscript),
}

#[derive(PartialEq, Eq, Clone, Copy, Debug)]
pub enum SpecialCharacter {
    Space,
    Infinity,
}
#[derive(new)]
pub struct Literal {
    text: String,
}
#[derive(new)]
pub struct Concatenation {
    children: Vec<FormatNode>,
}
#[derive(new)]
pub struct CommaSeparatedList {
    children: Vec<FormatNode>,
}
#[derive(new)]
pub struct OperatorName {
    child: Box<FormatNode>,
}
#[derive(new)]
pub struct Fraction {
    numerator: Box<FormatNode>,
    denominator: Box<FormatNode>,
}
#[derive(new)]
pub struct Subscript {
    child: Box<FormatNode>,
}
#[derive(new)]
pub struct Superscript {
    child: Box<FormatNode>,
}

impl fmt::Display for FormatNode {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let encoder = UnicodeEncoder::new();
        let context = Context {
            vpos: VPos::Normal,
            font_style: FontStyle::MathNormal,
        };
        write!(f, "{}", encoder.encode(self, &context))
    }
}


#[derive(Clone)]
pub struct Context {
    vpos: VPos,
    font_style: FontStyle,
}

// TODO: Is there a way to copy context only when it actually changed?
fn context_for_children(mut context: Context, node: &FormatNode) -> Context {
    use FormatNode as FN;
    match node {
        FN::Literal(_) |
        FN::SpecialCharacter(_) |
        FN::Concatenation(_) |
        FN::CommaSeparatedList(_) |
        FN::Fraction(_)
            => {},
        FN::OperatorName(_) => {
            context.font_style = FontStyle::MathStraight;
        }
        FN::Subscript(_) => {
            assert_eq!(context.vpos, VPos::Normal);
            context.vpos = VPos::Sub;
        },
        FN::Superscript(_) => {
            assert_eq!(context.vpos, VPos::Normal);
            context.vpos = VPos::Super;
        },
    }
    context
}


trait Encoder {
    fn encode(&self, node: &FormatNode, context: &Context) -> String;
}

#[derive(new)]
struct PlainTextEncoder {}
impl Encoder for PlainTextEncoder {
    fn encode(&self, node: &FormatNode, context: &Context) -> String {
        let child_context = context_for_children(context.clone(), &node);
        use FormatNode as FN;
        match node {
            FN::Literal(v) => v.text.clone(),
            FN::SpecialCharacter(v) => {
                use SpecialCharacter as SC;
                match v {
                    SC::Space => " ",
                    SC::Infinity => "Inf",
                }.to_string()
            }
            FN::Concatenation(v) => v.children.iter().map(|c| self.encode(c, &child_context)).join(""),
            FN::CommaSeparatedList(v) => {
                let mut children_encoded = v.children.iter().map(|c| self.encode(c, &child_context));
                match context.vpos {
                    VPos::Normal => children_encoded.join(","),
                    VPos::Sub => children_encoded.join("_"),
                    VPos::Super => children_encoded.join("^"),
                }
            },
            FN::OperatorName(v) => self.encode(&*v.child, &child_context),
            // TODO: Add a way to optionally add parens (here and in Unicode encoder).
            FN::Fraction(v) => format!(
                "{} / {}",
                self.encode(&*v.numerator, &child_context),
                self.encode(&*v.denominator, &child_context)
            ),
            FN::Subscript(v) => format!("_{}", self.encode(&*v.child, &child_context)),
            FN::Superscript(v) => format!("^{}", self.encode(&*v.child, &child_context)),
        }
    }
}

#[derive(new)]
struct UnicodeEncoder {}
impl Encoder for UnicodeEncoder {
    fn encode(&self, node: &FormatNode, context: &Context) -> String {
        let child_context = context_for_children(context.clone(), &node);
        use FormatNode as FN;
        match node {
            FN::Literal(v) => {
                match context.vpos {
                    VPos::Normal => v.text.clone(),
                    VPos::Sub => v.text.chars().into_iter().map(|c| unicode_to_subscript(c).unwrap()).collect(),
                    VPos::Super => v.text.chars().into_iter().map(|c| unicode_to_superscript(c).unwrap()).collect(),
                }
            }
            FN::SpecialCharacter(v) => {
                // TODO: Forbid custom context.vpos is other places where necessary.
                // TODO: Do this consistently with PlainTextEncoder.
                assert_eq!(context.vpos, VPos::Normal);
                use SpecialCharacter as SC;
                match v {
                    SC::Space => " ",
                    SC::Infinity => "∞",
                }.to_string()
            }
            FN::Concatenation(v) => v.children.iter().map(|c| self.encode(c, &child_context)).join(""),
            FN::CommaSeparatedList(v) => {
                let children_encoded = v.children.iter().map(|c| self.encode(c, &child_context)).collect_vec();
                let all_children_atomic = || children_encoded.iter().all(|c| c.len() == 1);
                match context.vpos {
                    VPos::Normal => children_encoded.join(","),
                    VPos::Sub => children_encoded.join(if all_children_atomic() { "," } else { "" }),
                    VPos::Super => children_encoded.join(if all_children_atomic() { "˒" } else { "" }),
                }
            },
            FN::OperatorName(v) => self.encode(&*v.child, &child_context),
            FN::Fraction(v) => format!(
                "{} / {}",
                self.encode(&*v.numerator, &child_context),
                self.encode(&*v.denominator, &child_context)
            ),
            FN::Subscript(v) => self.encode(&*v.child, &child_context),
            FN::Superscript(v) => self.encode(&*v.child, &child_context),
        }
    }
}


pub mod mfmt {
    use super::*;
    use FormatNode as FN;

    pub fn lit(text: String) -> FN { FN::Literal(Literal::new(text)) }
    pub fn concat(children: Vec<FN>) -> FN { FN::Concatenation(Concatenation::new(children)) }

    pub fn space() -> FN { FN::SpecialCharacter(SpecialCharacter::Space) }
    pub fn inf() -> FN { FN::SpecialCharacter(SpecialCharacter::Infinity) }

    pub fn frac(numerator: FN, denominator: FN) -> FN {
        FN::Fraction(Fraction::new(Box::new(numerator), Box::new(denominator)))
    }
    pub fn sub(child: FN) -> FN { FN::Subscript(Subscript::new(Box::new(child))) }
    pub fn sup(child: FN) -> FN { FN::Superscript(Superscript::new(Box::new(child))) }
}
