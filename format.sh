set -o nounset
set -o pipefail

thisfile="${BASH_SOURCE[0]}"
thisdir="$( cd "$( dirname "${thisfile}" )" && pwd )"

# TODO:
# how to make this print out the name of files it is processing?
find ${thisdir}/ -iname *.h -o -iname *.cc -iname *.hpp -iname *.cpp \
    | xargs \
    clang-format -style="{BasedOnStyle: webkit, IndentWidth: 4, AccessModifierOffset: -2}" -i
