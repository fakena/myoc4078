INCLUDE_DIRECTORIES (
	${OPENCOG_SOURCE_DIR}/src/util
	${OPENCOG_SOURCE_DIR}/src/atomspace
	${OPENCOG_SOURCE_DIR}/src/xml
	${OPENCOG_SOURCE_DIR}/src/server
	${EXPAT_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
	${OPENCOG_BINARY_DIR}/src/atomspace
	${OPENCOG_BINARY_DIR}/src/xml
	${OPENCOG_BINARY_DIR}/src/server
)

LINK_LIBRARIES(#CogServerUTest
	server
	${Boost_SIGNALS_LIBRARY}
)

ADD_CXXTEST(CogServerUTest)
ADD_CXXTEST(AgentUTest)
