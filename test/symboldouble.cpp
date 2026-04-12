#include <cmath>
#include <memory>
#include <sstream>
#include <tuple>

#include <gtest/gtest.h>

#include <olala/inputsequence.h>
#include <olala/inputstreamstream.h>
#include <olala/lookaheadstate.h>
#include <olala/parsercontext.h>
#include <olala/semanticstack.h>
#include <olala/symbol.h>
#include <olala/symboldouble.h>
#include <olala/valuedouble.h>

using namespace OLala;

namespace {

LookaheadStatus lookaheadDouble(const std::string& input) {
  auto stream(
      std::make_shared<InputStreamStream>(
          std::make_unique<std::istringstream>(input)));
  InputSequence seq;
  seq.pushStream(stream, "test");
  SemanticStack stack;
  const ParserContext ctx(&seq, &stack);
  SymbolDouble sym;
  return sym.lookahead(ctx);
}

/* -- Parse a floating-point literal, pop the ValueDouble from the stack. */
std::shared_ptr<ValueDouble> parseDouble(
    const std::string& input,
    InputSequence& seq,
    SemanticStack& stack) {
  auto stream(
      std::make_shared<InputStreamStream>(
          std::make_unique<std::istringstream>(input)));
  seq.pushStream(stream, "test");
  const ParserContext ctx(&seq, &stack);
  SymbolDouble sym;
  sym.parse(ctx, nullptr);
  auto [vptr, range] = stack.popValue();
  return std::dynamic_pointer_cast<ValueDouble>(vptr);
}

} // namespace

/* -- lookahead accept/reject tests ---------------------------------------- */

TEST(SymbolDouble, DecimalWithDot) {
  ASSERT_EQ(lookaheadDouble("3.14+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, DecimalDotOnly) {
  /* -- "1." is a valid float literal */
  ASSERT_EQ(lookaheadDouble("1.+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, DecimalStartingWithDot) {
  ASSERT_EQ(lookaheadDouble(".5+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, DecimalWithExponent) {
  ASSERT_EQ(lookaheadDouble("1e10+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, DecimalWithSignedExponent) {
  ASSERT_EQ(lookaheadDouble("1.5e+3+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, DecimalWithNegativeExponent) {
  ASSERT_EQ(lookaheadDouble("2.5e-2+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, DecimalWithFloatSuffix) {
  ASSERT_EQ(lookaheadDouble("1.5f+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, DecimalWithFloatSuffixUpper) {
  ASSERT_EQ(lookaheadDouble("1.5F+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, DecimalWithLongSuffix) {
  ASSERT_EQ(lookaheadDouble("1.5l+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, DecimalWithLongSuffixUpper) {
  ASSERT_EQ(lookaheadDouble("1.5L+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, HexWithExponent) {
  ASSERT_EQ(lookaheadDouble("0x1p0+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, HexFractional) {
  ASSERT_EQ(lookaheadDouble("0x1.8p+1+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, HexStartingWithDot) {
  ASSERT_EQ(lookaheadDouble("0x.8p-1+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, HexUpperPrefix) {
  ASSERT_EQ(lookaheadDouble("0X1p0+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, HexWithLongSuffix) {
  ASSERT_EQ(lookaheadDouble("0x1p0l+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolDouble, IntegerLiteral) {
  /* -- plain integer: no dot, no exponent */
  ASSERT_EQ(lookaheadDouble("42+").result, LookaheadResult::REJECT);
}

TEST(SymbolDouble, JustDot) {
  ASSERT_EQ(lookaheadDouble(".+").result, LookaheadResult::REJECT);
}

TEST(SymbolDouble, HexWithoutExponent) {
  /* -- hex float requires binary exponent */
  ASSERT_EQ(lookaheadDouble("0x1.8+").result, LookaheadResult::REJECT);
}

TEST(SymbolDouble, Letters) {
  ASSERT_EQ(lookaheadDouble("abc+").result, LookaheadResult::REJECT);
}

TEST(SymbolDouble, EmptyInput) {
  ASSERT_EQ(lookaheadDouble("").result, LookaheadResult::REJECT);
}

/* -- value and is_long tests ---------------------------------------------- */

TEST(SymbolDouble, ValueDecimal) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseDouble("3.14+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_NEAR(static_cast<double>(val->getValue()), 3.14, 1e-9);
  ASSERT_EQ(val->getKind(), ValueDouble::Kind::DOUBLE);
}

TEST(SymbolDouble, KindDoubleNoSuffix) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseDouble("1.0+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getKind(), ValueDouble::Kind::DOUBLE);
}

TEST(SymbolDouble, KindFloatLower) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseDouble("1.0f+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getKind(), ValueDouble::Kind::FLOAT);
}

TEST(SymbolDouble, KindFloatUpper) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseDouble("1.0F+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getKind(), ValueDouble::Kind::FLOAT);
}

TEST(SymbolDouble, KindLongDoubleLower) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseDouble("1.0l+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getKind(), ValueDouble::Kind::LONG_DOUBLE);
}

TEST(SymbolDouble, KindLongDoubleUpper) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseDouble("1.0L+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getKind(), ValueDouble::Kind::LONG_DOUBLE);
}

TEST(SymbolDouble, ValueExponent) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseDouble("1.5e2+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_NEAR(static_cast<double>(val->getValue()), 150.0, 1e-9);
}

TEST(SymbolDouble, DoesNotConsumeTrailingChar) {
  auto stream(
      std::make_shared<InputStreamStream>(
          std::make_unique<std::istringstream>("3.14+")));
  InputSequence seq;
  seq.pushStream(stream, "test");
  SemanticStack stack;
  const ParserContext ctx(&seq, &stack);

  SymbolDouble sym;
  sym.parse(ctx, nullptr);

  auto range(seq.openRange());
  ASSERT_EQ(range.fetchCharacter(), U'+');
}
