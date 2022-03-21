use std::iter::Peekable;

use phf::phf_map;


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
    Placeholder(),
    Literal(char),
    Command(LatexCommand),
    Sequence(Vec<LatexToken>),
}

const fn num_command_args(cmd: MathCommand) -> i32 {
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


fn parse_latex_atom<I: Iterator<Item = Lexeme>>(lexemes: &mut Peekable<I>) -> LatexToken {
    let l = lexemes.next().expect("Unexpected end-of-input in parse_latex_atom");
    match l {
        Lexeme::Symbol(symb) => {
            match symb {
                '{' => panic!("{}", "Unexpected '{' (note: braces are allowed only around command argument and for sub-/super-script)"),
                '@' => LatexToken::Placeholder(),
                _ => LatexToken::Literal(symb),
            }
        },
        Lexeme::Command(cmd) => {
            let num_args = num_command_args(cmd);
            let mut args = Vec::new();
            for _ in 0..num_args {
                args.push(parse_latex_block(lexemes))
            }
            LatexToken::Command(LatexCommand{
                command: cmd,
                args: args
            })
        },
    }
}

fn parse_latex_block<I: Iterator<Item = Lexeme>>(lexemes: &mut Peekable<I>) -> LatexToken {
    if *lexemes.peek().expect("Unexpected end-of-input in parse_latex_block") != Lexeme::Symbol('{') {
        return parse_latex_atom(lexemes);
    }
    lexemes.next();
    let mut tokens = Vec::new();
    while let Some(l) = lexemes.peek() {
        if *l == Lexeme::Symbol('}') {
            lexemes.next();
            return LatexToken::Sequence(tokens);
        }
        tokens.push(parse_latex_atom(lexemes));
    }
    panic!("{}", "No matching '}'");
}

pub fn parse_latex(src: &str) -> LatexToken {
    let mut lexemes = to_lexemes(src).into_iter().peekable();
    let mut tokens = Vec::new();
    while lexemes.peek().is_some() {
        tokens.push(parse_latex_block(&mut lexemes));
    }
    LatexToken::Sequence(tokens)
}


#[derive(PartialEq, Eq, Debug)]
enum Lexeme {
    Symbol(char),
    Command(MathCommand),
}

impl Lexeme {
    pub fn from_command_name(cmd: &str) -> Self {
        match NAME_TO_COMMAND.get(cmd) {
            Some(v) => Lexeme::Command(*v),
            None => panic!("Unknown command: '{}'", cmd),
        }
    }
}

fn to_lexemes(src: &str) -> Vec<Lexeme> {
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
                ret.push(Lexeme::from_command_name(&command));
            }
            just_saw_backslash = false;
        } else if let Some(mut command) = current_command {
            if ch.is_ascii_alphabetic() {
                command.push(ch);
                current_command = Some(command)
            } else {
                ret.push(Lexeme::from_command_name(&command));
                current_command = None;
                src_iter.put_back(ch);
            }
        } else if ch.is_ascii_whitespace() {
            // skip
        } else if ch == '\\' {
            just_saw_backslash = true;
        } else if ch == '_' || ch == '^' {
            ret.push(Lexeme::from_command_name(&String::from(ch)));
        } else {
            ret.push(Lexeme::Symbol(ch));
        }
    }
    if let Some(command) = current_command {
        ret.push(Lexeme::from_command_name(&command));
    }
    assert!(!just_saw_backslash);
    ret
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
            vec![
                Command(Fraction),
                Symbol('1'),
                Symbol('{'),
                Symbol('2'),
                Symbol('3'),
                Command(Superscript),
                Symbol('@'),
                Symbol('}'),
            ]
        );
        assert_eq!(
            to_lexemes(r"a\   \  b c\ "),
            vec![
                Symbol('a'),
                Command(Space),
                Command(Space),
                Symbol('b'),
                Symbol('c'),
                Command(Space),
            ]
        );
    }

    #[test]
    fn parse_latex_test() {
        use LatexToken::{Placeholder, Literal, Command, Sequence};
        use MathCommand::*;
        assert_eq!(
            parse_latex(r"\frac1{23^@}"),
            Sequence(vec![
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
                                    Placeholder(),
                                ]
                            })
                        ])
                    ]
                })
            ])
        );
    }
}
