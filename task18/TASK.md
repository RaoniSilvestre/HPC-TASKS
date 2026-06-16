Tarefa 18: Multiplicação de Matrizes com dados derivados		

Reimplemente a tarefa 17, agora distribuindo as colunas entre os processos. 

Utilize apenas em uma versão MPI_Type_vector e em outra acrescente MPI_Type_create_resized para definir um tipo derivado que represente colunas da matriz. 

Use MPI_Scatter com esse tipo para distribuir blocos de colunas, e MPI_Scatter para enviar os segmentos correspondentes de x. 

Cada processo deve calcular uma contribuição parcial para todos os elementos de y e usa MPI_Reduce com MPI_SUM para somar os vetores parciais no processo 0. 

Discuta as diferenças de acesso à memória e desempenho em relação à distribuição por linhas. Analise o desempenho das versões da tarefa 17, e das duas dessa tarefa.
