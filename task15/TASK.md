# Tarefa 15: Simulação da difusão de calor		

Implemente uma simulação da difusão de calor em uma barra 1D, dividida entre dois ou mais processos MPI. Cada processo deve simular um trecho da barra com células extras para troca de bordas com vizinhos. 

Implemente três versões: uma com MPI_Send/ MPI_Recv, outra com MPI_Isend/ MPI_Irecv e MPI_Wait, e uma terceira usando MPI_Test para atualizar os pontos internos enquanto aguarda a comunicação. Compare os tempos de execução e discuta os ganhos com sobreposição de comunicação e computação.
