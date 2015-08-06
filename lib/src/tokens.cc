#include <internal/tokens.hpp>

using namespace std;

namespace hocon {

    /** Value token */
    value::value(unique_ptr<abstract_config_value> value) :
            token(token_type::VALUE, nullptr, value->transform_to_string()), _value(move(value)) { }

    value::value(unique_ptr<abstract_config_value> value, string original_text) :
            token(token_type::VALUE, nullptr, original_text),
            _value(move(value)) { }

    std::string value::to_string() const {
        return _value->transform_to_string();
    }

    shared_ptr<simple_config_origin> value::origin() const {
        return _value->origin();
    }

    bool value::operator==(const token& other) const {
        return other.get_token_type() == token_type::VALUE &&
               other.to_string() == to_string();
    }


    /** Line token */
    line::line(shared_ptr<simple_config_origin> origin) :
            token(token_type::NEWLINE, move(origin), "\n") { }

    string line::to_string() const {
        return "'\\n'@" + std::to_string(line_number());
    }

    bool line::operator==(const token& other) const {
        return (other.get_token_type() == token_type::NEWLINE) && (line_number() == other.line_number());
    }

    /** Unquoted text token */
    unquoted_text::unquoted_text(shared_ptr<simple_config_origin> origin, string text) :
            token(token_type::UNQUOTED_TEXT, move(origin), move(text)) { }

    string unquoted_text::to_string() const {
        return "'" + token_text() + "' (UNQUOTED)";
    }

    bool unquoted_text::operator==(const token& other) const {
        return (other.get_token_type() == token_type::UNQUOTED_TEXT) &&
                (other.token_text() == token_text());
    }

    /** Ignored whitespace token */
    ignored_whitespace::ignored_whitespace(shared_ptr<simple_config_origin> origin, string whitespace) :
        token(token_type::IGNORED_WHITESPACE, move(origin), move(whitespace)) { }

    string ignored_whitespace::to_string() const {
        return "'" + token_text() + "' (WHITESPACE)";
    }

    bool ignored_whitespace::operator==(const token& other) const {
        return other.get_token_type() == token_type::IGNORED_WHITESPACE &&
                other.token_text() == token_text();
    }

    /** Problem token */
    problem::problem(shared_ptr<simple_config_origin> origin, string what, string message,
        bool suggest_quotes) : token(token_type::PROBLEM, move(origin)), _what(move(what)),
        _message(move(message)), _suggest_quotes(suggest_quotes) { }

    string problem::what() const {
        return _what;
    }

    string problem::message() const {
        return _message;
    }

    bool problem::suggest_quotes() const {
        return _suggest_quotes;
    }

    std::string problem::to_string() const {
        return "'" + _what + "' (" + _message + ")";
    }

    bool problem::operator==(const token& other) const {
        try {
            problem const& other_problem = dynamic_cast<problem const&>(other);
            return other_problem.what() == what() &&
                   other_problem.message() == message() &&
                   other_problem.suggest_quotes() == suggest_quotes();
        } catch (bad_cast ex){
            return false;
        }
    }

    /** Comment token */
    comment::comment(shared_ptr<simple_config_origin> origin, string text) :
        token(token_type::COMMENT, move(origin)), _text(move(text)) { }

    string comment::text() const {
        return _text;
    }

    string comment::to_string() const {
        return "'#" + _text + "' (COMMENT)";
    }

    bool comment::operator==(const token& other) const {
        return other.get_token_type() == token_type::COMMENT && to_string() == other.to_string();
    }

    /** Double-slash comment token */
    double_slash_comment::double_slash_comment(shared_ptr<simple_config_origin> origin, string text) :
        comment(move(origin), move(text)) { }

    string double_slash_comment::token_text() const {
        return "//" + text();
    }

    /** Hash comment token */
    hash_comment::hash_comment(shared_ptr<simple_config_origin> origin, string text) :
        comment(move(origin), move(text)) { }

    string hash_comment::token_text() const {
        return "#" + text();
    }

    /** Substitution token */
    substitution::substitution(shared_ptr<simple_config_origin> origin, bool optional,
        vector<shared_ptr<token>> expression) : token(token_type::SUBSTITUTION, move(origin)), _optional(optional),
        _expression(move(expression)) { }

    bool substitution::optional() const {
        return _optional;
    }

    vector<shared_ptr<token>> const& substitution::expression() const {
        return _expression;
    }

    string substitution::token_text() const {
        // TODO: this relies on tokenizer working to port properly
        return to_string();
    }

    string substitution::to_string() const {
        string result;
        for (auto&& t : _expression) {
            result += t->token_text();
        }
        return "'${" + result + "}'";
    }

    bool substitution::operator==(const token& other) const {
        return other.get_token_type() == token_type::SUBSTITUTION &&
               to_string() == other.to_string();
    }

    /** Singleton tokens */
    shared_ptr<token> tokens::start_token() {
        static shared_ptr<token> _start = make_shared<token>(
                token_type::START, nullptr, "start of file", "");
        return _start;
    }

    shared_ptr<token> tokens::end_token() {
        static shared_ptr<token> _end = make_shared<token>(
                token_type::END, nullptr, "start of file", "");
        return _end;
    }

    shared_ptr<token> tokens::comma_token() {
        static shared_ptr<token> _comma = make_shared<token>(
                token_type::COMMA, nullptr, "','", ",");
        return _comma;
    }

    shared_ptr<token> tokens::equals_token() {
        static shared_ptr<token> _equals = make_shared<token>(
                token_type::EQUALS, nullptr, "'='", "=");
        return _equals;
    }

    shared_ptr<token> tokens::colon_token() {
        static shared_ptr<token> _colon = make_shared<token>(
                token_type::COLON, nullptr, "':'", ":");
        return _colon;
    }

    shared_ptr<token> tokens::open_curly_token() {
        static shared_ptr<token> _open_curly = make_shared<token>(
                token_type::OPEN_CURLY, nullptr, "'{'", "{");
        return _open_curly;
    }

    shared_ptr<token> tokens::close_curly_token() {
        static shared_ptr<token> _close_curly = make_shared<token>(
                token_type::CLOSE_CURLY, nullptr, "'}'", "}");
        return _close_curly;
    }

    shared_ptr<token> tokens::open_square_token() {
        static shared_ptr<token> _open_square = make_shared<token>(
                token_type::OPEN_SQUARE, nullptr, "'['", "[");
        return _open_square;
    }

    shared_ptr<token> tokens::close_square_token() {
        static shared_ptr<token> _close_square = make_shared<token>(
                token_type::CLOSE_SQUARE, nullptr, "']'", "]");
        return _close_square;
    }

    shared_ptr<token> tokens::plus_equals_token() {
        static shared_ptr<token> _plus_equals = make_shared<token>(
                token_type::PLUS_EQUALS, nullptr, "'+='", "+=");
        return _plus_equals;
    }

}  // namespace hocon
