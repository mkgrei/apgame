#!/bin/bash

if [ -z "${APGAME_AGENT_BUILD_TYPE+x}" ]; then
  echo "APGAME_AGENT_BUILD_TYPE undefined"
  exit 1
fi

if [ ${APGAME_AGENT_BUILD_TYPE} = "cxx" ]; then
  if [ -z "${APGAME_AGENT_SOURCE_DIR+x}" ]; then
    echo "APGAME_AGENT_SOURCE_DIR undefined"
    exit 1
  fi

  if [ -z "${APGAME_AGENT_BUILD_DIR+x}" ]; then
    echo "APGAME_AGENT_BUILD_DIR undefined"
    exit 1
  fi

  if [ -z "${APGAME_CXX_SOURCE_NAME+x}" ]; then
    echo "APGAME_CXX_SOURCE_NAME undefined"
    exit 1
  fi
  if [ -z "${APGAME_CXX_EXECUTABLE_NAME+x}" ]; then
    echo "APGAME_CXX_EXECUTABLE_NAME undefined"
    exit 1
  fi
  if [ -z "${APGAME_CXX_COMPILER_NAME+x}" ]; then
    echo "APGAME_CXX_COMPILER_NAME undefined"
    exit 1
  fi
  if [ -z "${APGAME_CXX_COMPILER_FLAG+x}" ]; then
    echo "APGAME_CXX_COMPILER_FLAG undefined"
    exit 1
  fi

  COMPILER_NAME=${APGAME_CXX_COMPILER_NAME}
  COMPILER_FLAG=${APGAME_CXX_COMPILER_FLAG}
  SOURCE_FILE=${APGAME_AGENT_SOURCE_DIR}/${APGAME_CXX_SOURCE_NAME}
  EXECUTABLE_FILE=${APGAME_AGENT_BUILD_DIR}/${APGAME_CXX_EXECUTABLE_NAME}
  eval "${COMPILER_NAME} ${COMPILER_FLAG} ${SOURCE_FILE} -o ${EXECUTABLE_FILE}"

else
  echo "unknown BUILD_TYPE=${APGAME_AGENT_BUILD_TYPE}"
  exit 1
fi
