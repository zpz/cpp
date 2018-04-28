set -o nounset
set -o pipefail

thisfile="${BASH_SOURCE[0]}"
thisdir="$( cd "$( dirname "${thisfile}" )" && pwd )"

astyle --style=kr --indent-modifiers --indent-switches --pad-oper --add-braces --preserve-date --recursive --suffix=none ${thisdir}/"*.h, *.cc, *.hpp, *.cpp"
