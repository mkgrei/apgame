FROM pritunl/archlinux:latest

RUN yes | pacman -S gcc && \
    yes | pacman -S boost

COPY include /usr/include
COPY agent-build.bash /usr/bin/agent-build.bash
