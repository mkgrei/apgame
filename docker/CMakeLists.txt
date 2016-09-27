cmake_minimum_required(VERSION 2.7)

function(add_docker_target target_name docker_name)
  add_custom_target(${target_name}
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/${docker_name} ${CMAKE_CURRENT_BINARY_DIR}/${docker_name}
    COMMAND cp -r ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/${docker_name}
    COMMAND cp ${CMAKE_SOURCE_DIR}/bin/agent-build.bash ${CMAKE_CURRENT_BINARY_DIR}/${docker_name}
    COMMAND docker build
      -t ${docker_name} ${CMAKE_CURRENT_BINARY_DIR}/${docker_name}
  )
  add_dependencies(docker ${target_name})
endfunction(add_docker_target)

add_custom_target(docker)

add_docker_target(docker-archlinux-basic archlinux-basic)
