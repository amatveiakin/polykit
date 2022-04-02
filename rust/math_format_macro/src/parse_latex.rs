use std::iter::Peekable;

use phf::phf_map;


#[derive(PartialEq, Eq, Debug)]
pub struct ParsingError {
    pub message: String,
    // TODO: Store location and show it in compilation error message:
    //   https://stackoverflow.com/questions/71663743/make-procedural-macro-compilation-error-point-inside-a-token
}
impl ParsingError {
    pub fn new<T: ToString>(message: T) -> Self { Self{ message: message.to_string() } }
}

#[derive(PartialEq, Eq, Clone, Copy, Debug)]
pub enum PlaceholderFormat {
    Atom,
    List,
}

#[derive(PartialEq, Eq, Debug)]
pub struct Placeholder {
    pub format: PlaceholderFormat,
    pub arg_index: usize,
}

#[derive(PartialEq, Eq, Clone, Copy, Debug)]
pub enum MathCommand {
    Subscript,
    Superscript,
    OperatorName,
    Fraction,

    Space,
    Infinity,

    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    Orange,
    Gray,
    PaleRed,
    PaleGreen,
    PaleBlue,
    PaleMagenta,
    PaleCyan,
}

#[derive(PartialEq, Eq, Debug)]
pub struct Command {
    pub command: MathCommand,
    pub args: Vec<Token>,
}

#[derive(PartialEq, Eq, Debug)]
pub enum Token {
    Placeholder(Placeholder),
    Literal(char),
    Command(Command),
    Sequence(Vec<Token>),
}

#[derive(PartialEq, Eq, Debug)]
pub struct Document {
    pub root: Token,
    pub num_placeholders: usize,
}

pub const fn num_command_args(cmd: MathCommand) -> i32 {
    match cmd {
        MathCommand::Subscript => 1,
        MathCommand::Superscript => 1,
        MathCommand::OperatorName => 1,
        MathCommand::Fraction => 2,

        MathCommand::Space => 0,
        MathCommand::Infinity => 0,

        MathCommand::Red => 1,
        MathCommand::Green => 1,
        MathCommand::Yellow => 1,
        MathCommand::Blue => 1,
        MathCommand::Magenta => 1,
        MathCommand::Cyan => 1,
        MathCommand::Orange => 1,
        MathCommand::Gray => 1,
        MathCommand::PaleRed => 1,
        MathCommand::PaleGreen => 1,
        MathCommand::PaleBlue => 1,
        MathCommand::PaleMagenta => 1,
        MathCommand::PaleCyan => 1,
    }
}

const NAME_TO_COMMAND: phf::Map<&'static str, MathCommand> = phf_map! {
    "_" => MathCommand::Subscript,
    "^" => MathCommand::Superscript,
    "op" => MathCommand::OperatorName,
    "frac" => MathCommand::Fraction,

    " " => MathCommand::Space,
    "inf" => MathCommand::Infinity,

    "red" => MathCommand::Red,
    "green" => MathCommand::Green,
    "yellow" => MathCommand::Yellow,
    "blue" => MathCommand::Blue,
    "magenta" => MathCommand::Magenta,
    "cyan" => MathCommand::Cyan,
    "orange" => MathCommand::Orange,
    "gray" => MathCommand::Gray,
    "palered" => MathCommand::PaleRed,
    "palegreen" => MathCommand::PaleGreen,
    "paleblue" => MathCommand::PaleBlue,
    "palemagenta" => MathCommand::PaleMagenta,
    "palecyan" => MathCommand::PaleCyan,
};

const FORMAT_TO_PLACEHOLDER: phf::Map<&'static str, PlaceholderFormat> = phf_map! {
    "" => PlaceholderFormat::Atom,
    "," => PlaceholderFormat::List,
};


fn parse_atom<I>(lexemes: &mut Peekable<I>) -> Result<Token, ParsingError>
where
    I: Iterator<Item = Lexeme>
{
    let l = lexemes.next().ok_or(ParsingError::new("Unexpected end-of-input in parse_atom"))?;
    match l {
        Lexeme::Symbol(symb) => {
            match symb {
                '{' => Err(ParsingError::new("Unexpected '{' (note: braces are allowed only around command argument and for sub-/super-script)")),
                _ => Ok(Token::Literal(symb)),
            }
        },
        Lexeme::Placeholder(placeholder) => {
            Ok(Token::Placeholder(placeholder))
        },
        Lexeme::Command(cmd) => {
            let num_args = num_command_args(cmd);
            let mut args = Vec::new();
            for _ in 0..num_args {
                args.push(parse_block(lexemes)?)
            }
            Ok(Token::Command(Command{
                command: cmd,
                args: args
            }))
        },
    }
}

fn parse_block<I>(lexemes: &mut Peekable<I>) -> Result<Token, ParsingError>
where
    I: Iterator<Item = Lexeme>
{
    let next_lexeme = lexemes.peek().ok_or(ParsingError::new("Unexpected end-of-input in parse_block"))?;
    if *next_lexeme != Lexeme::Symbol('{') {
        return parse_atom(lexemes);
    }
    lexemes.next();
    let mut tokens = Vec::new();
    while let Some(l) = lexemes.peek() {
        if *l == Lexeme::Symbol('}') {
            lexemes.next();
            return Ok(Token::Sequence(tokens));
        }
        tokens.push(parse_atom(lexemes)?);
    }
    Err(ParsingError::new("No matching '}'"))
}

pub fn parse_document(src: &str) -> Result<Document, ParsingError> {
    let (lexemes, num_placeholders) = to_lexemes(src)?;
    let mut lexemes_iter = lexemes.into_iter().peekable();
    let mut tokens = Vec::new();
    while lexemes_iter.peek().is_some() {
        let token = parse_block(&mut lexemes_iter)?;
        tokens.push(token);
    }
    Ok(Document {
        root: Token::Sequence(tokens),
        num_placeholders: num_placeholders,
    })
}


#[derive(PartialEq, Eq, Debug)]
enum Lexeme {
    Symbol(char),
    Command(MathCommand),
    Placeholder(Placeholder),
}

impl Lexeme {
    pub fn from_command_name(cmd: &str) -> Result<Self, ParsingError> {
        match NAME_TO_COMMAND.get(cmd) {
            Some(v) => Ok(Lexeme::Command(*v)),
            None => Err(ParsingError::new(format!("Unknown command: '{}'", cmd))),
        }
    }
}

pub fn parse_placeholder_format(format: &str) -> Result<PlaceholderFormat, ParsingError> {
    match FORMAT_TO_PLACEHOLDER.get(format) {
        Some(v) => Ok(*v),
        None => Err(ParsingError::new(format!("Unknown placeholder format: '{}'", format))),
    }
}

fn to_lexemes(src: &str) -> Result<(Vec<Lexeme>, usize), ParsingError> {
    let mut current_command = None;
    let mut just_saw_backslash = false;
    let mut current_placeholder: Option<String> = None;
    let mut next_placeholder_id = 0;
    let mut ret = Vec::new();
    let mut src_iter = itertools::put_back(src.chars());
    while let Some(ch) = src_iter.next() {
        if let Some(mut placeholder) = current_placeholder {
            if ch == '>' {
                ret.push(Lexeme::Placeholder(Placeholder {
                    format: parse_placeholder_format(&placeholder)?,
                    arg_index: next_placeholder_id,
                }));
                next_placeholder_id += 1;
                current_placeholder = None;
            } else {
                placeholder.push(ch);
                current_placeholder = Some(placeholder);
            }
        } else if just_saw_backslash {
            assert!(current_command.is_none());
            let command = String::from(ch);
            if ch.is_ascii_alphabetic() {
                current_command = Some(command);
            } else {
                ret.push(Lexeme::from_command_name(&command)?);
            }
            just_saw_backslash = false;
        } else if let Some(mut command) = current_command {
            if ch.is_ascii_alphabetic() {
                command.push(ch);
                current_command = Some(command)
            } else {
                ret.push(Lexeme::from_command_name(&command)?);
                current_command = None;
                src_iter.put_back(ch);
            }
        } else if ch.is_ascii_whitespace() {
            // skip
        } else if ch == '\\' {
            just_saw_backslash = true;
        } else if ch == '_' || ch == '^' {
            ret.push(Lexeme::from_command_name(&String::from(ch))?);
        } else if ch == '<' {
            current_placeholder = Some(String::new());
        } else if ch == '>' {
            return Err(ParsingError::new("Unmatched '>'"));
        } else {
            ret.push(Lexeme::Symbol(ch));
        }
    }
    if let Some(command) = current_command {
        ret.push(Lexeme::from_command_name(&command)?);
    }
    if just_saw_backslash {
        return Err(ParsingError::new("Format string cannot end with '\\'"));
    }
    Ok((ret, next_placeholder_id))
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn to_lexemes_test() {
        use self::MathCommand::*;
        assert_eq!(
            to_lexemes(r"\frac1{23^<>}"),
            Ok((
                vec![
                    Lexeme::Command(Fraction),
                    Lexeme::Symbol('1'),
                    Lexeme::Symbol('{'),
                    Lexeme::Symbol('2'),
                    Lexeme::Symbol('3'),
                    Lexeme::Command(Superscript),
                    Lexeme::Placeholder(Placeholder {
                        format: PlaceholderFormat::Atom,
                        arg_index: 0,
                    }),
                    Lexeme::Symbol('}'),
                ],
                1
            ))
        );
        assert_eq!(
            to_lexemes(r"a\   \  b c\ "),
            Ok((
                vec![
                    Lexeme::Symbol('a'),
                    Lexeme::Command(Space),
                    Lexeme::Command(Space),
                    Lexeme::Symbol('b'),
                    Lexeme::Symbol('c'),
                    Lexeme::Command(Space),
                ],
                0
            ))
        );
    }

    #[test]
    fn parse_document_test() {
        use self::MathCommand::*;
        assert_eq!(
            parse_document(r"\frac1{23^<>} + <>"),
            Ok(Document {
                root: Token::Sequence(vec![
                    Token::Command(Command {
                        command: Fraction,
                        args: vec![
                            Token::Literal('1'),
                            Token::Sequence(vec![
                                Token::Literal('2'),
                                Token::Literal('3'),
                                Token::Command(Command {
                                    command: Superscript,
                                    args: vec![
                                        Token::Placeholder(Placeholder {
                                            format: PlaceholderFormat::Atom,
                                            arg_index: 0,
                                        }),
                                    ]
                                })
                            ])
                        ]
                    }),
                    Token::Literal('+'),
                    Token::Placeholder(Placeholder {
                        format: PlaceholderFormat::Atom,
                        arg_index: 1,
                    }),
                ]),
                num_placeholders: 2,
            })
        );
    }
}
