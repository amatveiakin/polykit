extern crate console;
extern crate derive_new;
extern crate itertools;
extern crate once_cell;

mod unicode_alphabets;

use std::fmt;
use std::rc::Rc;
use std::sync::Mutex;

use console::style;
use derive_new::new;
use itertools::Itertools;
use once_cell::sync::Lazy;

use unicode_alphabets::{unicode_to_subscript, unicode_to_superscript};


static CONFIG_STACK: Lazy<Mutex<Vec<FormattingConfig>>> = Lazy::new(|| {
    Mutex::new(vec![FormattingConfig {
        encoder: Encoder::Ascii,
        rich_text_format: RichTextFormat::Console,
        include_annotations: true,
    }])
});

#[derive(PartialEq, Eq, Clone, Copy, Debug)]
pub enum Encoder {
    Ascii,
    Unicode,
    Latex,
}

// Rich text support diagram:
//
//           |              Rich text format
//  Encoder  +-------------------------------------------
//           |  Disabled    Console     HTML       LaTeX
// ----------+-------------------------------------------
//   ASCII   |  disabled    enabled    enabled      N/A
//  Unicode  |  disabled    enabled    enabled      N/A
//   LaTeX   |  disabled      N/A        N/A      enabled
//
#[derive(PartialEq, Eq, Clone, Copy, Debug)]
pub enum RichTextFormat {
    Disabled,
    Console,
    Html,
    Latex,
}

#[derive(Clone, Debug)]
pub struct FormattingConfig {
    pub encoder: Encoder,
    pub rich_text_format: RichTextFormat,
    pub include_annotations: bool,
}

#[derive(Clone, Default, Debug)]
pub struct FormattingConfigOverrides {
    pub encoder: Option<Encoder>,
    pub rich_text_format: Option<RichTextFormat>,
    pub include_annotations: Option<bool>,
}

macro_rules! override_options {
    (($old:ident <= $new:ident) -> $t:tt : $($field:ident,)*) => {
        $t { $(
            $field: if let Some(new_value) = $new.$field { new_value } else { $old.$field },
        )* }
    }
}

fn override_formatting_config(config: FormattingConfig, overrides: FormattingConfigOverrides) -> FormattingConfig {
    override_options!((config <= overrides) -> FormattingConfig:
        encoder,
        rich_text_format,
        include_annotations,
    )
}

fn check_formatting_config(config: &FormattingConfig) {
    use RichTextFormat as RTF;
    match (config.encoder, config.rich_text_format) {
        (_, RTF::Disabled) => {},
        (Encoder::Ascii | Encoder::Unicode, RTF::Console | RTF::Html) => {},
        (Encoder::Latex, RTF::Latex) => {},
        _ => panic!(
            "Unsupported combination: Encoder={:?}, RichTextFormat={:?}",
            config.encoder, config.rich_text_format
        ),
    }
}

pub fn current_formatting_config() -> FormattingConfig {
    Lazy::force(&CONFIG_STACK).lock().unwrap().last().unwrap().clone()
}

pub struct ScopedFormatting;
impl ScopedFormatting {
    pub fn new(overrides: FormattingConfigOverrides) -> Self {
        let mut stack = Lazy::force(&CONFIG_STACK).lock().unwrap();
        let current_config = stack.last().unwrap().clone();
        let new_config = override_formatting_config(current_config, overrides);
        check_formatting_config(&new_config);
        stack.push(new_config);
        Self{}
    }
}
impl Drop for ScopedFormatting {
    fn drop(&mut self) {
        let mut stack = Lazy::force(&CONFIG_STACK).lock().unwrap();
        stack.pop();
    }
}

// Changes formatting config until the end of the scope.
#[macro_export]
macro_rules! scoped_formatting {
    ($($field:ident : $value:expr),* $(,)?) => {
        let _scoped_formatting = ScopedFormatting::new(FormattingConfigOverrides {
            $($field: Some($value),)*
            ..FormattingConfigOverrides::default()
        });
    };
}


pub trait MathFormat {
    fn to_format_node(self) -> FormatNode;
}
impl MathFormat for FormatNode {
    fn to_format_node(self) -> FormatNode { self }
}
impl<T: fmt::Display> MathFormat for T {
    fn to_format_node(self) -> FormatNode { FormatNode::Literal(Literal::new(self.to_string())) }
}

#[derive(PartialEq, Eq, Clone, Copy, Debug)]
enum VPos {
    Normal,
    Sub,
    Super,
}

#[derive(PartialEq, Eq, Clone, Copy, Debug)]
enum FontStyle {
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

impl FormatNode {
    pub fn render(&self) -> String {
        let config = current_formatting_config();
        let encoder = encoder_for_config(&config);
        let context = Context {
            config: Rc::new(config),
            vpos: VPos::Normal,
            font_style: FontStyle::MathNormal,
        };
        encoder.encode(self, &context)
    }
}


#[derive(Clone)]
struct Context {
    config: Rc<FormattingConfig>,
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

// TODO: Allow to choose formatting option: concole, html, latex or none
fn format_fragment_as_math_normal(fragment: &str, rich_text_format: RichTextFormat, is_alpha: bool) -> String {
    if is_alpha {
        use RichTextFormat as RTF;
        match rich_text_format {
            RTF::Disabled | RTF::Latex => fragment.to_owned(),
            RTF::Console => {
                // This should've been italic, but it's not supported on many terminals including
                //   Windows cmd. So using gray color instead.
                // Note. Another approach for Unicode encoder would've been to use italic Unicode
                //   letters (https://unicode-search.net/unicode-namesearch.pl?term=ITALIC,
                //   https://yaytext.com/bold-italic/). Sadly, terminal support is terrible.
                // TODO: What if it's nested inside a colored fragment?
                format!("{}", style(&fragment).color256(8))
            },
            RTF::Html => {
                format!("<i>{}</i>", fragment)
            },
        }
    } else {
        fragment.to_owned()
    }
}

fn format_literal(s: &str, context: &Context) -> String {
    match context.font_style {
        FontStyle::MathNormal => {
            let mut ret = String::new();
            let mut fragment = String::new();
            let mut fragment_is_alpha = None;
            for ch in s.chars() {
                let ch_is_alpha = Some(ch.is_alphabetic());
                if fragment_is_alpha != ch_is_alpha {
                    match fragment_is_alpha {
                        None => assert!(fragment.is_empty()),
                        Some(is_alpha) => ret.push_str(
                            &format_fragment_as_math_normal(&fragment, context.config.rich_text_format, is_alpha)
                        ),
                    }
                    fragment.clear();
                    fragment_is_alpha = ch_is_alpha;
                }
                fragment.push(ch);
            }
            match fragment_is_alpha {
                None => assert!(fragment.is_empty()),
                Some(is_alpha) => ret.push_str(
                    &format_fragment_as_math_normal(&fragment, context.config.rich_text_format, is_alpha)
                ),
    }
            ret
        },
        FontStyle::MathStraight => s.to_owned(),
    }
}


fn encoder_for_config(config: &FormattingConfig) -> Box<dyn EncoderInterface> {
    // TODO: Reuse encoders
    match config.encoder {
        Encoder::Ascii => Box::new(AsciiEncoder::new()),
        Encoder::Unicode => Box::new(UnicodeEncoder::new()),
        Encoder::Latex => panic!("Encoder::Latex not implemented"),
    }
}

trait EncoderInterface {
    fn encode(&self, node: &FormatNode, context: &Context) -> String;
}

#[derive(new)]
struct AsciiEncoder;
impl EncoderInterface for AsciiEncoder {
    fn encode(&self, node: &FormatNode, context: &Context) -> String {
        let child_context = context_for_children(context.clone(), &node);
        use FormatNode as FN;
        match node {
            FN::Literal(v) => format_literal(&v.text, context),
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
struct UnicodeEncoder;
impl EncoderInterface for UnicodeEncoder {
    fn encode(&self, node: &FormatNode, context: &Context) -> String {
        let child_context = context_for_children(context.clone(), &node);
        use FormatNode as FN;
        match node {
            FN::Literal(v) => {
                match context.vpos {
                    VPos::Normal => format_literal(&v.text, context),
                    VPos::Sub => v.text.chars().into_iter().map(|c| unicode_to_subscript(c).unwrap()).collect(),
                    VPos::Super => v.text.chars().into_iter().map(|c| unicode_to_superscript(c).unwrap()).collect(),
                }
            }
            FN::SpecialCharacter(v) => {
                // TODO: Forbid custom context.vpos is other places where necessary.
                // TODO: Do this consistently with AsciiEncoder.
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
                // Technically speaking, `graphemes` from the unicode-segmentation crate would be better,
                //   but `chars` are good enough since there is no upper/lower case diactrics.
                let all_children_atomic = || children_encoded.iter().all(|c| c.chars().count() == 1);
                match context.vpos {
                    VPos::Normal => children_encoded.join(","),
                    VPos::Sub => children_encoded.join(if all_children_atomic() { "" } else { "," }),
                    VPos::Super => children_encoded.join(if all_children_atomic() { "" } else { "˒" }),
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
    use MathFormat as MF;
    use FormatNode as FN;

    pub fn lit<T: MF>(v: T) -> FN { v.to_format_node() }
    pub fn comma_list<T: MF>(children: Vec<T>) -> FN {
        FN::CommaSeparatedList(CommaSeparatedList::new(vec_to_format_nodes(children)))
    }
    pub fn concat<T: MF>(children: Vec<T>) -> FN {
        FN::Concatenation(Concatenation::new(vec_to_format_nodes(children)))
    }

    pub fn space() -> FN { FN::SpecialCharacter(SpecialCharacter::Space) }
    pub fn inf() -> FN { FN::SpecialCharacter(SpecialCharacter::Infinity) }

    pub fn op<T: MF>(child: T) -> FN { FN::OperatorName(OperatorName::new(Box::new(child.to_format_node()))) }
    pub fn frac<T1: MF, T2: MF>(numerator: T1, denominator: T2) -> FN {
        FN::Fraction(Fraction::new(Box::new(numerator.to_format_node()), Box::new(denominator.to_format_node())))
    }
    pub fn sub<T: MF>(child: T) -> FN { FN::Subscript(Subscript::new(Box::new(child.to_format_node()))) }
    pub fn sup<T: MF>(child: T) -> FN { FN::Superscript(Superscript::new(Box::new(child.to_format_node()))) }

    fn vec_to_format_nodes<T: MF>(elements: Vec<T>) -> Vec<FN> {
        elements.into_iter().map(|c| c.to_format_node()).collect()
    }
}
