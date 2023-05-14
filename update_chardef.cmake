file(REMOVE_RECURSE ${BINARY_DIR}/handfont/chardefs)
file(COPY ${CURRENT_SOURCE_DIR}/chardefs/files DESTINATION ${BINARY_DIR}/handfont)
file(RENAME ${BINARY_DIR}/handfont/files ${BINARY_DIR}/handfont/chardefs)
file(REMOVE ${BINARY_DIR}/handfont/files)
