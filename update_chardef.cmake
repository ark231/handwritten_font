file(REMOVE_RECURSE ${BINARY_DIR}/handfont/chardefs/standard)
file(COPY ${CURRENT_SOURCE_DIR}/chardefs/files DESTINATION ${BINARY_DIR}/handfont/chardefs)
file(RENAME ${BINARY_DIR}/handfont/chardefs/files ${BINARY_DIR}/handfont/chardefs/standard)
file(REMOVE ${BINARY_DIR}/handfont/chardefs/files)
