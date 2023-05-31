FROM gcc:latest

COPY main.cpp /usr/src/main.cpp

WORKDIR /usr/src

RUN g++ -o main main.cpp

CMD ["./main"]