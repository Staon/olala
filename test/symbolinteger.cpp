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
#include <olala/symbolinteger.h>
#include <olala/valueinteger.h>

using namespace OLala;

namespace {

LookaheadStatus lookaheadInteger(const std::string& input) {
  auto stream(
      std::make_shared<InputStreamStream>(
          std::make_unique<std::istringstream>(input)));
  InputSequence seq;
  seq.pushStream(stream, "test");
  SemanticStack stack;
  const ParserContext ctx(&seq, &stack);
  SymbolInteger sym;
  return sym.lookahead(ctx);
}

std::shared_ptr<ValueInteger> parseInteger(
    const std::string& input,
    InputSequence& seq,
    SemanticStack& stack) {
  auto stream(
      std::make_shared<InputStreamStream>(
          std::make_unique<std::istringstream>(input)));
  seq.pushStream(stream, "test");
  const ParserContext ctx(&seq, &stack);
  SymbolInteger sym;
  sym.parse(ctx, nullptr);
  auto [vptr, range] = stack.popValue();
  return std::dynamic_pointer_cast<ValueInteger>(vptr);
}

} // namespace

/* -- lookahead accept/reject tests ---------------------------------------- */

TEST(SymbolInteger, Decimal) {
  ASSERT_EQ(lookaheadInteger("42+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, Zero) {
  ASSERT_EQ(lookaheadInteger("0+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, Octal) {
  ASSERT_EQ(lookaheadInteger("0755+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, Hexadecimal) {
  ASSERT_EQ(lookaheadInteger("0xff+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, HexadecimalUpper) {
  ASSERT_EQ(lookaheadInteger("0xDEAD+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, HexadecimalUpperPrefix) {
  ASSERT_EQ(lookaheadInteger("0Xff+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, Binary) {
  ASSERT_EQ(lookaheadInteger("0b1010+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, BinaryUpperPrefix) {
  ASSERT_EQ(lookaheadInteger("0B1010+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, SuffixUnsigned) {
  ASSERT_EQ(lookaheadInteger("42u+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, SuffixLong) {
  ASSERT_EQ(lookaheadInteger("42l+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, SuffixLongLong) {
  ASSERT_EQ(lookaheadInteger("42ll+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, SuffixUnsignedLong) {
  ASSERT_EQ(lookaheadInteger("42ul+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, SuffixLongUnsigned) {
  ASSERT_EQ(lookaheadInteger("42lu+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, SuffixUnsignedLongLong) {
  ASSERT_EQ(lookaheadInteger("42ull+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, SuffixLongLongUnsigned) {
  ASSERT_EQ(lookaheadInteger("42llu+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, SuffixUpperULL) {
  ASSERT_EQ(lookaheadInteger("42ULL+").result, LookaheadResult::ACCEPT);
}

TEST(SymbolInteger, Letters) {
  ASSERT_EQ(lookaheadInteger("abc+").result, LookaheadResult::REJECT);
}

TEST(SymbolInteger, StartingWithDot) {
  ASSERT_EQ(lookaheadInteger(".5+").result, LookaheadResult::REJECT);
}

TEST(SymbolInteger, EmptyInput) {
  ASSERT_EQ(lookaheadInteger("").result, LookaheadResult::REJECT);
}

/* -- value and kind tests ------------------------------------------------- */

TEST(SymbolInteger, ValueDecimal) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseInteger("42+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getValue(), 42ULL);
  ASSERT_EQ(val->getKind(), ValueInteger::Kind::INT);
}

TEST(SymbolInteger, ValueZero) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseInteger("0+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getValue(), 0ULL);
  ASSERT_EQ(val->getKind(), ValueInteger::Kind::INT);
}

TEST(SymbolInteger, ValueOctal) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseInteger("0755+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getValue(), 0755ULL);
}

TEST(SymbolInteger, ValueHex) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseInteger("0xff+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getValue(), 0xffULL);
}

TEST(SymbolInteger, ValueBinary) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseInteger("0b101+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getValue(), 5ULL);
}

TEST(SymbolInteger, KindUnsignedInt) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseInteger("42u+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getKind(), ValueInteger::Kind::UNSIGNED_INT);
}

TEST(SymbolInteger, KindLong) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseInteger("42l+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getKind(), ValueInteger::Kind::LONG);
}

TEST(SymbolInteger, KindLongLong) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseInteger("42ll+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getKind(), ValueInteger::Kind::LONG_LONG);
}

TEST(SymbolInteger, KindUnsignedLongFromUL) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseInteger("42ul+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getKind(), ValueInteger::Kind::UNSIGNED_LONG);
}

TEST(SymbolInteger, KindUnsignedLongFromLU) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseInteger("42lu+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getKind(), ValueInteger::Kind::UNSIGNED_LONG);
}

TEST(SymbolInteger, KindUnsignedLongLongFromULL) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseInteger("42ull+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getKind(), ValueInteger::Kind::UNSIGNED_LONG_LONG);
}

TEST(SymbolInteger, KindUnsignedLongLongFromLLU) {
  InputSequence seq;
  SemanticStack stack;
  auto val = parseInteger("42llu+", seq, stack);
  ASSERT_NE(val, nullptr);
  ASSERT_EQ(val->getKind(), ValueInteger::Kind::UNSIGNED_LONG_LONG);
}

TEST(SymbolInteger, DoesNotConsumeTrailingChar) {
  auto stream(
      std::make_shared<InputStreamStream>(
          std::make_unique<std::istringstream>("42+")));
  InputSequence seq;
  seq.pushStream(stream, "test");
  SemanticStack stack;
  const ParserContext ctx(&seq, &stack);

  SymbolInteger sym;
  sym.parse(ctx, nullptr);

  auto range(seq.openRange());
  ASSERT_EQ(range.fetchCharacter(), U'+');
}
