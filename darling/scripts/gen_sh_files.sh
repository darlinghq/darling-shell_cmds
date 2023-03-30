SRCROOT="$(cd ../.. && pwd)"
BUILT_PRODUCTS_DIR="$SRCROOT/gen/sh"
DERIVED_FILE_DIR="$SRCROOT/gen/tmp"

mkdir -p $BUILT_PRODUCTS_DIR

# mkbuiltins
cd ${BUILT_PRODUCTS_DIR} && sh ${SRCROOT}/sh/mkbuiltins ${SRCROOT}/sh

# mknodes
mkdir -p ${DERIVED_FILE_DIR}
env -i xcrun -sdk macosx cc ${SRCROOT}/sh/mknodes.c -o ${DERIVED_FILE_DIR}/mknodes
cd ${BUILT_PRODUCTS_DIR} && ${DERIVED_FILE_DIR}/mknodes ${SRCROOT}/sh/nodetypes ${SRCROOT}/sh/nodes.c.pat

# mksyntax
env -i xcrun -sdk macosx cc ${SRCROOT}/sh/mksyntax.c -o ${DERIVED_FILE_DIR}/mksyntax
cd ${BUILT_PRODUCTS_DIR} && ${DERIVED_FILE_DIR}/mksyntax

# mktokens
cd ${BUILT_PRODUCTS_DIR} && sh ${SRCROOT}/sh/mktokens
