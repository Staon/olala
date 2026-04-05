#include <memory>
#include <sstream>

#include <gtest/gtest.h>

#include <olala/inputsequence.h>
#include <olala/inputstreamstream.h>
#include <olala/lookaheadstate.h>
#include <olala/parsercontext.h>
#include <olala/semanticstack.h>
#include <olala/symbol.h>
#include <olala/symbolidentifier.h>

using namespace OLala;

namespace {

LookaheadStatus lookaheadIdentifier(const std::string& input) {
  auto stream(
      std::make_shared<InputStreamStream>(
          std::make_unique<std::istringstream>(input)));
  InputSequence seq;
  seq.pushStream(stream, "test");
  SemanticStack stack;
  const ParserContext ctx(&seq, &stack);
  SymbolIdentifier ident;
  return ident.lookahead(ctx);
}

} // namespace

TEST(SymbolIdentifier, SimpleIdentifier) {
  auto status = lookaheadIdentifier("foo+");
  ASSERT_EQ(status.result, LookaheadResult::ACCEPT);
}

TEST(SymbolIdentifier, IdentifierWithUnderscoreStart) {
  auto status = lookaheadIdentifier("_bar+");
  ASSERT_EQ(status.result, LookaheadResult::ACCEPT);
}

TEST(SymbolIdentifier, IdentifierWithDigits) {
  auto status = lookaheadIdentifier("myVar2+");
  ASSERT_EQ(status.result, LookaheadResult::ACCEPT);
}

TEST(SymbolIdentifier, StartsWithDigit) {
  auto status = lookaheadIdentifier("2bad+");
  ASSERT_EQ(status.result, LookaheadResult::REJECT);
}

TEST(SymbolIdentifier, SingleUnderscore) {
  auto status = lookaheadIdentifier("_+");
  ASSERT_EQ(status.result, LookaheadResult::ACCEPT);
}

TEST(SymbolIdentifier, SingleLetter) {
  auto status = lookaheadIdentifier("x+");
  ASSERT_EQ(status.result, LookaheadResult::ACCEPT);
}

TEST(SymbolIdentifier, EmptyInput) {
  auto status = lookaheadIdentifier("");
  ASSERT_EQ(status.result, LookaheadResult::REJECT);
}
