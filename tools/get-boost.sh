#! /bin/sh

set -e

branch="master"
if [ "$1" = "master" ]; then
    branch="develop"
fi

variant="debug"
if [ "$BEAST_LOUNGE_BUILD_TYPE" = "Release" ]; then
    variant="release"
fi

git clone -b $branch --depth 1 https://github.com/boostorg/boost.git boost-root

cd boost-root

git submodule update --init \
    libs/headers \
    tools/build \
    tools/boost_install \
    libs/algorithm \
    libs/array \
    libs/asio \
    libs/assert \
    libs/atomic \
    libs/beast \
    libs/bind \
    libs/chrono \
    libs/concept_check \
    libs/config \
    libs/container \
    libs/container_hash \
    libs/core \
    libs/date_time \
    libs/detail \
    libs/endian \
    libs/exception \
    libs/function \
    libs/functional \
    libs/integer \
    libs/intrusive \
    libs/io \
    libs/iterator \
    libs/lexical_cast \
    libs/locale \
    libs/logic \
    libs/math \
    libs/move \
    libs/mp11 \
    libs/mpl \
    libs/numeric/conversion \
    libs/optional \
    libs/predef \
    libs/preprocessor \
    libs/range \
    libs/ratio \
    libs/smart_ptr \
    libs/static_assert \
    libs/system \
    libs/thread \
    libs/throw_exception \
    libs/tuple \
    libs/type_index \
    libs/type_traits \
    libs/utility \
    libs/align

CXX= ./bootstrap.sh
./b2 install --with-system --with-thread variant=$variant cxxstd=11 threadapi=pthread link=static

echo Submodule update complete
