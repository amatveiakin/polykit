use std::iter::Peekable;

use phf::phf_map;


#[derive(PartialEq, Eq, Debug)]
pub struct LatexParsingError {
    pub message: String,
    // TODO: Store location and show it in compilation error message:
    //   https://stackoverflow.com/questions/71663743/make-procedural-macro-compilation-error-point-inside-a-token
}
impl LatexParsingError {
    pub fn new<T: ToString>(message: T) -> Self { Self{ message: message.to_string() } }
}

#[derive(PartialEq, Eq, Clone, Copy, Debug)]
pub enum MathCommand {
    Subscript,
    Superscript,
    Fraction,
    Space,
    Infinity,
}

#[derive(PartialEq, Eq, Debug)]
pub struct LatexCommand {
    pub command: MathCommand,
    pub args: Vec<LatexToken>,
}

#[derive(PartialEq, Eq, Debug)]
pub enum LatexToken {
    Placeholder(usize),  // placeholder position in the initial string
    Literal(char),
    Command(LatexCommand),
    Sequence(Vec<LatexToken>),
}

#[derive(PartialEq, Eq, Debug)]
pub struct LatexDocument {
    pub root: LatexToken,
    pub num_placeholders: usize,
}

pub const fn num_command_args(cmd: MathCommand) -> i32 {
    match cmd {
        MathCommand::Subscript => 1,
        MathCommand::Superscript => 1,
        MathCommand::Fraction => 2,
        MathCommand::Space => 0,
        MathCommand::Infinity => 0,
    }
}

const NAME_TO_COMMAND: phf::Map<&'static str, MathCommand> = phf_map! {
    "_" => MathCommand::Subscript,
    "^" => MathCommand::Superscript,
    "frac" => MathCommand::Fraction,
    " " => MathCommand::Space,
    "inf" => MathCommand::Infinity,
};


fn parse_latex_atom<I>(lexemes: &mut Peekable<I>, next_placeholder_id: &mut usize) -> Result<LatexToken, LatexParsingError>
where
    I: Iterator<Item = Lexeme>
{
    let l = lexemes.next().ok_or(LatexParsingError::new("Unexpected end-of-input in parse_latex_atom"))?;
    match l {
        Lexeme::Symbol(symb) => {
            match symb {
                '{' => Err(LatexParsingError::new("Unexpected '{' (note: braces are allowed only around command argument and for sub-/super-script)")),
                '@' => {
                    let placeholder_id = *next_placeholder_id;
                    *next_placeholder_id += 1;
                    Ok(LatexToken::Placeholder(placeholder_id))
                },
                _ => Ok(LatexToken::Literal(symb)),
            }
        },
        Lexeme::Command(cmd) => {
            let num_args = num_command_args(cmd);
            let mut args = Vec::new();
            for _ in 0..num_args {
                args.push(parse_latex_block(lexemes, next_placeholder_id)?)
            }
            Ok(LatexToken::Command(LatexCommand{
                command: cmd,
                args: args
            }))
        },
    }
}

fn parse_latex_block<I>(lexemes: &mut Peekable<I>, next_placeholder_id: &mut usize) -> Result<LatexToken, LatexParsingError>
where
    I: Iterator<Item = Lexeme>
{
    let next_lexeme = lexemes.peek().ok_or(LatexParsingError::new("Unexpected end-of-input in parse_latex_block"))?;
    if *next_lexeme != Lexeme::Symbol('{') {
        return parse_latex_atom(lexemes, next_placeholder_id);
    }
    lexemes.next();
    let mut tokens = Vec::new();
    while let Some(l) = lexemes.peek() {
        if *l == Lexeme::Symbol('}') {
            lexemes.next();
            return Ok(LatexToken::Sequence(tokens));
        }
        tokens.push(parse_latex_atom(lexemes, next_placeholder_id)?);
    }
    Err(LatexParsingError::new("No matching '}'"))
}

pub fn parse_latex(src: &str) -> Result<LatexDocument, LatexParsingError> {
    let mut lexemes = to_lexemes(src)?.into_iter().peekable();
    let mut tokens = Vec::new();
    let mut next_placeholder_id: usize = 0;
    while lexemes.peek().is_some() {
        let token = parse_latex_block(&mut lexemes, &mut next_placeholder_id)?;
        tokens.push(token);
    }
    Ok(LatexDocument {
        root: LatexToken::Sequence(tokens),
        num_placeholders: next_placeholder_id,
    })
}


#[derive(PartialEq, Eq, Debug)]
enum Lexeme {
    Symbol(char),
    Command(MathCommand),
}

impl Lexeme {
    pub fn from_command_name(cmd: &str) -> Result<Self, LatexParsingError> {
        match NAME_TO_COMMAND.get(cmd) {
            Some(v) => Ok(Lexeme::Command(*v)),
            None => Err(LatexParsingError::new(format!("Unknown command: '{}'", cmd))),
        }
    }
}

fn to_lexemes(src: &str) -> Result<Vec<Lexeme>, LatexParsingError> {
    let mut current_command = None;
    let mut just_saw_backslash = false;
    let mut ret = Vec::new();
    let mut src_iter = itertools::put_back(src.chars());
    while let Some(ch) = src_iter.next() {
        if just_saw_backslash {
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
        } else {
            ret.push(Lexeme::Symbol(ch));
        }
    }
    if let Some(command) = current_command {
        ret.push(Lexeme::from_command_name(&command)?);
    }
    if just_saw_backslash {
        return Err(LatexParsingError::new("Format string cannot end with '\\'"));
    }
    Ok(ret)
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn to_lexemes_test() {
        use self::Lexeme::{Command, Symbol};
        use MathCommand::*;
        assert_eq!(
            to_lexemes(r"\frac1{23^@}"),
            Ok(vec![
                Command(Fraction),
                Symbol('1'),
                Symbol('{'),
                Symbol('2'),
                Symbol('3'),
                Command(Superscript),
                Symbol('@'),
                Symbol('}'),
            ])
        );
        assert_eq!(
            to_lexemes(r"a\   \  b c\ "),
            Ok(vec![
                Symbol('a'),
                Command(Space),
                Command(Space),
                Symbol('b'),
                Symbol('c'),
                Command(Space),
            ])
        );
    }

    #[test]
    fn parse_latex_test() {
        use LatexToken::{Placeholder, Literal, Command, Sequence};
        use MathCommand::*;
        assert_eq!(
            parse_latex(r"\frac1{23^@} + @"),
            Ok(LatexDocument {
                root: Sequence(vec![
                    Command(LatexCommand {
                        command: Fraction,
                        args: vec![
                            Literal('1'),
                            Sequence(vec![
                                Literal('2'),
                                Literal('3'),
                                Command(LatexCommand {
                                    command: Superscript,
                                    args: vec![
                                        Placeholder(0),
                                    ]
                                })
                            ])
                        ]
                    }),
                    Literal('+'),
                    Placeholder(1),
                ]),
                num_placeholders: 2,
            })
        );
    }
}
