# UnixTermProject2023
2023-2 Unix Programming Term Project

## 🚀 요구사항
- 계산 및 I/O 속도를 높이기 위한 병렬작업(parallel operation)을 구현하기 위한 목적임
- Static library를 사용해서 구현할 것 (template 참조, 시간 계산을 위한 compiler directive 사용)
- 4 processes, 4개의 node file을 사용하며, 전체 data크기는 4MB임 (1M of integer)
- 전체 데이터가 (*, cyclic)으로 분산되어 있으며, 이에 대한 client_oriented_io, server_oriented_io를 구현 (io는 write임)
- 두 기법의 통신 기법이 달라야 함 (pipe, fifo, message queue, shared memory, socket중 두 개를 선택)
- 구현 및 점검을 쉽게 하도록 A[k] = k로 함
- Compute node에서 연산된 데이터들을 I/O node에서 모으는 두 가지 병렬 I/O (parallel I/O) 방식을 각각 다른 통신 수단을 사용하여 실험하고 성능을 비교함

## Client-oriented collective I/O
각 client(compute node)가 주어진 영역의 데이터들을 다른 client로 부터 모아서 연속적인 데이터들을 보내는 방법

## Server-oriented collective I/O
각 server(I/O node)가 주어진 영역의 데이터들을 client로 부터 모아서 저장하는 방법

## 실행 방법
```
$ make
$ ./mytest
```
## 결과
![image](https://github.com/parkblo/UnixTermProject2023/assets/62245207/0cad8c15-84f7-448f-a671-860c5724fe6c)
