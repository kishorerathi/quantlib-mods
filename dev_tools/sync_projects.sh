#!/bin/bash

# execute this script from the root of an uncompressed QuantLib tarball

# get reference lists of distributed files (done with find; this is
# why this script should be run from an uncompressed tarball created
# with 'make dist', not from a working copy.)

find ql -name '*.[hc]pp' -or -name '*.[hc]' \
| grep -v 'ql/config\.hpp' | sort > ql.ref.files
find test-suite -name '*.[hc]pp' | grep -v 'quantlibbenchmark' \
| sort > test-suite.ref.files

# extract file names from VC7 projects and clean up so that they
# have the same format as the reference lists.

grep -o -E 'RelativePath=".*"' QuantLib_vc7.vcproj \
| awk -F'"' '{ print $2 }' | sed -e 's|\\|/|g' | sed -e 's|^./||' \
| sort > ql.vc7.files

grep -o -E 'RelativePath=".*">' test-suite/testsuite_vc7.vcproj \
| awk -F'"' '{ print $2 }' | sed -e 's|\\|/|g' | sed -e 's|^./||' \
| sed -e 's|^|test-suite/|' | sort > test-suite.vc7.files

# Same for VC8...

grep -o -E 'RelativePath=".*"' QuantLib_vc8.vcproj \
| awk -F'"' '{ print $2 }' | sed -e 's|\\|/|g' | sed -e 's|^./||' \
| sort > ql.vc8.files

grep -o -E 'RelativePath=".*"' test-suite/testsuite_vc8.vcproj \
| awk -F'"' '{ print $2 }' | sed -e 's|\\|/|g' | sed -e 's|^./||' \
| sed -e 's|^|test-suite/|' | sort > test-suite.vc8.files

# ...VC9...

grep -o -E 'RelativePath=".*"' QuantLib_vc9.vcproj \
| awk -F'"' '{ print $2 }' | sed -e 's|\\|/|g' | sed -e 's|^./||' \
| sort > ql.vc9.files

grep -o -E 'RelativePath=".*"' test-suite/testsuite_vc9.vcproj \
| awk -F'"' '{ print $2 }' | sed -e 's|\\|/|g' | sed -e 's|^./||' \
| sed -e 's|^|test-suite/|' | sort > test-suite.vc9.files

# ...VC10...

grep -o -E 'Include=".*\.[hc]p*"' QuantLib_vc10.vcxproj \
| awk -F'"' '{ print $2 }' | sed -e 's|\\|/|g' | sed -e 's|^./||' \
| sort > ql.vc10.files

grep -o -E 'Include=".*\.[hc]p*"' test-suite/testsuite_vc10.vcxproj \
| awk -F'"' '{ print $2 }' | sed -e 's|\\|/|g' | sed -e 's|^./||' \
| sed -e 's|^|test-suite/|' | sort > test-suite.vc10.files

grep -o -E 'Include=".*\.[hc]p*"' QuantLib_vc10.vcxproj.filters \
| awk -F'"' '{ print $2 }' | sed -e 's|\\|/|g' | sed -e 's|^./||' \
| sort > ql.vc10.filters

grep -o -E 'Include=".*\.[hc]p*"' test-suite/testsuite_vc10.vcxproj.filters \
| awk -F'"' '{ print $2 }' | sed -e 's|\\|/|g' | sed -e 's|^./||' \
| sed -e 's|^|test-suite/|' | sort > test-suite.vc10.filters

# ...and Dev-C++.

grep -o -E 'FileName=.*' QuantLib.dev \
| grep -v 'QuantLib\.dev' \
| awk -F'=' '{ print $2 }' | sed -e 's|\\|/|g' | sed -e 's|^./||' \
| sort > ql.devcpp.files

grep -o -E 'FileName=.*' test-suite/testsuite.dev \
| grep -v 'testsuite\.dev' \
| awk -F'=' '{ print $2 }' | sed -e 's|\\|/|g' | sed -e 's|^./||' \
| sed -e 's|^|test-suite/|' | sort > test-suite.devcpp.files

# write out differences...

echo 'Visual Studio 7:' > sync.report
diff -b ql.vc7.files ql.ref.files >> sync.report
diff -b test-suite.vc7.files test-suite.ref.files >> sync.report

echo '' >> sync.report
echo '' >> sync.report
echo 'Visual Studio 8:' >> sync.report
diff -b ql.vc8.files ql.ref.files >> sync.report
diff -b test-suite.vc8.files test-suite.ref.files >> sync.report

echo '' >> sync.report
echo '' >> sync.report
echo 'Visual Studio 9:' >> sync.report
diff -b ql.vc9.files ql.ref.files >> sync.report
diff -b test-suite.vc9.files test-suite.ref.files >> sync.report

echo '' >> sync.report
echo '' >> sync.report
echo 'Visual Studio 10:' >> sync.report
echo 'project:' >> sync.report
diff -b ql.vc10.files ql.ref.files >> sync.report
diff -b test-suite.vc10.files test-suite.ref.files >> sync.report
echo 'filters:' >> sync.report
diff -b ql.vc10.filters ql.ref.files >> sync.report
diff -b test-suite.vc10.filters test-suite.ref.files >> sync.report

echo '' >> sync.report
echo '' >> sync.report
echo 'Dev-C++:' >> sync.report
diff -b ql.devcpp.files ql.ref.files >> sync.report
diff -b test-suite.devcpp.files test-suite.ref.files >> sync.report

# ...and cleanup
rm -f ql.ref.files test-suite.ref.files
rm -f ql.vc7.files test-suite.vc7.files
rm -f ql.vc8.files test-suite.vc8.files
rm -f ql.vc9.files test-suite.vc9.files
rm -f ql.vc10.files test-suite.vc10.files
rm -f ql.vc10.filters test-suite.vc10.filters
rm -f ql.devcpp.files test-suite.devcpp.files

cat sync.report

