#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-slg");
    BOOST_CHECK(GetBoolArg("-slg"));
    BOOST_CHECK(GetBoolArg("-slg", false));
    BOOST_CHECK(GetBoolArg("-slg", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-slgo"));
    BOOST_CHECK(!GetBoolArg("-slgo", false));
    BOOST_CHECK(GetBoolArg("-slgo", true));

    ResetArgs("-slg=0");
    BOOST_CHECK(!GetBoolArg("-slg"));
    BOOST_CHECK(!GetBoolArg("-slg", false));
    BOOST_CHECK(!GetBoolArg("-slg", true));

    ResetArgs("-slg=1");
    BOOST_CHECK(GetBoolArg("-slg"));
    BOOST_CHECK(GetBoolArg("-slg", false));
    BOOST_CHECK(GetBoolArg("-slg", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noslg");
    BOOST_CHECK(!GetBoolArg("-slg"));
    BOOST_CHECK(!GetBoolArg("-slg", false));
    BOOST_CHECK(!GetBoolArg("-slg", true));

    ResetArgs("-noslg=1");
    BOOST_CHECK(!GetBoolArg("-slg"));
    BOOST_CHECK(!GetBoolArg("-slg", false));
    BOOST_CHECK(!GetBoolArg("-slg", true));

    ResetArgs("-slg -noslg");  // -slg should win
    BOOST_CHECK(GetBoolArg("-slg"));
    BOOST_CHECK(GetBoolArg("-slg", false));
    BOOST_CHECK(GetBoolArg("-slg", true));

    ResetArgs("-slg=1 -noslg=1");  // -slg should win
    BOOST_CHECK(GetBoolArg("-slg"));
    BOOST_CHECK(GetBoolArg("-slg", false));
    BOOST_CHECK(GetBoolArg("-slg", true));

    ResetArgs("-slg=0 -noslg=0");  // -slg should win
    BOOST_CHECK(!GetBoolArg("-slg"));
    BOOST_CHECK(!GetBoolArg("-slg", false));
    BOOST_CHECK(!GetBoolArg("-slg", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--slg=1");
    BOOST_CHECK(GetBoolArg("-slg"));
    BOOST_CHECK(GetBoolArg("-slg", false));
    BOOST_CHECK(GetBoolArg("-slg", true));

    ResetArgs("--noslg=1");
    BOOST_CHECK(!GetBoolArg("-slg"));
    BOOST_CHECK(!GetBoolArg("-slg", false));
    BOOST_CHECK(!GetBoolArg("-slg", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-slg", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-slg", "eleven"), "eleven");

    ResetArgs("-slg -slg");
    BOOST_CHECK_EQUAL(GetArg("-slg", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-slg", "eleven"), "");

    ResetArgs("-slg=");
    BOOST_CHECK_EQUAL(GetArg("-slg", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-slg", "eleven"), "");

    ResetArgs("-slg=11");
    BOOST_CHECK_EQUAL(GetArg("-slg", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-slg", "eleven"), "11");

    ResetArgs("-slg=eleven");
    BOOST_CHECK_EQUAL(GetArg("-slg", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-slg", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-slg", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-slg", 0), 0);

    ResetArgs("-slg -slg");
    BOOST_CHECK_EQUAL(GetArg("-slg", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-slg", 11), 0);

    ResetArgs("-slg=11 -slg=12");
    BOOST_CHECK_EQUAL(GetArg("-slg", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-slg", 11), 12);

    ResetArgs("-slg=NaN -slg=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-slg", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-slg", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--slg");
    BOOST_CHECK_EQUAL(GetBoolArg("-slg"), true);

    ResetArgs("--slg=verbose --slg=1");
    BOOST_CHECK_EQUAL(GetArg("-slg", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-slg", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noslg");
    BOOST_CHECK(!GetBoolArg("-slg"));
    BOOST_CHECK(!GetBoolArg("-slg", true));
    BOOST_CHECK(!GetBoolArg("-slg", false));

    ResetArgs("-noslg=1");
    BOOST_CHECK(!GetBoolArg("-slg"));
    BOOST_CHECK(!GetBoolArg("-slg", true));
    BOOST_CHECK(!GetBoolArg("-slg", false));

    ResetArgs("-noslg=0");
    BOOST_CHECK(GetBoolArg("-slg"));
    BOOST_CHECK(GetBoolArg("-slg", true));
    BOOST_CHECK(GetBoolArg("-slg", false));

    ResetArgs("-slg --noslg");
    BOOST_CHECK(GetBoolArg("-slg"));

    ResetArgs("-noslg -slg"); // slg always wins:
    BOOST_CHECK(GetBoolArg("-slg"));
}

BOOST_AUTO_TEST_SUITE_END()
