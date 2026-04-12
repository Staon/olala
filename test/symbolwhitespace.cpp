#include <memory>
#include <sstream>

#include <gtest/gtest.h>

#include <olala/inputsequence.h>
#include <olala/inputstreamstream.h>
#include <olala/lookaheadstate.h>
#include <olala/parsercontext.h>
#include <olala/semanticstack.h>
#include <olala/symbol.h>
#include <olala/symbolwhitespace.h>

using namespace OLala;

namespace {

LookaheadStatus lookaheadWhitespace(const std::string& input) {
  auto stream(
      std::make_shared<InputStreamStream>(
          std::make_unique<std::istringstream>(input)));
  InputSequence seq;
  seq.pushStream(stream, "test");
  SemanticStack stack;
  const ParserContext ctx(&seq, &stack);
  SymbolWhitespace ws;
  return ws.lookahead(ctx);
}

} // namespace

TEST(SymbolWhitespace, SingleSpace) {
  auto status = lookaheadWhitespace(" x");
  ASSERT_EQ(status.result, LookaheadResult::ACCEPT);
}

TEST(SymbolWhitespace, MultipleSpaces) {
  auto status = lookaheadWhitespace("   x");
  ASSERT_EQ(status.result, LookaheadResult::ACCEPT);
}

TEST(SymbolWhitespace, MixedWhitespace) {
  auto status = lookaheadWhitespace(" \t\r\n  x");
  ASSERT_EQ(status.result, LookaheadResult::ACCEPT);
}

TEST(SymbolWhitespace, NonWhitespace) {
  auto status = lookaheadWhitespace("abc");
  ASSERT_EQ(status.result, LookaheadResult::REJECT);
}

TEST(SymbolWhitespace, EmptyInput) {
  auto status = lookaheadWhitespace("");
  ASSERT_EQ(status.result, LookaheadResult::REJECT);
}

TEST(SymbolWhitespace, DoesNotConsumeTrailingChar) {
  /* -- After parsing whitespace, the first non-whitespace character
   * must still be available in the input. */
  auto stream(
      std::make_shared<InputStreamStream>(
          std::make_unique<std::istringstream>("  foo")));
  InputSequence seq;
  seq.pushStream(stream, "test");
  SemanticStack stack;
  const ParserContext ctx(&seq, &stack);

  SymbolWhitespace ws;
  ws.parse(ctx, nullptr);

  /* -- 'f' must be the next character in the sequence */
  auto range(seq.openRange());
  ASSERT_EQ(range.fetchCharacter(), U'f');
}
