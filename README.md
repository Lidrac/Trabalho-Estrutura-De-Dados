# Sistema de Gerenciamento de Pet Shop - Estrutura de Dados

Este projeto foi desenvolvido como trabalho final da disciplina de Estrutura de Dados. Trata-se de um sistema gerenciador de banco de dados para um Pet Shop, capaz de interpretar comandos textuais (estilo SQL), processá-los via filas sincronizadas e persistir os dados em arquivos binários.

## 🚀 Funcionalidades Principais

- **Processamento Assíncrono via Filas:** Leitura de um arquivo de texto (`comandos.txt`), distribuição dos comandos em filas específicas (Pessoa, Pet, Tipo) e execução sincronizada.
- **CRUD Completo:** Inserção, Leitura, Atualização e Remoção de dados.
- **Consultas Ordenadas (`SELECT`):** Utilização de **Árvores Binárias de Busca** para ordenar os resultados por diversos critérios (Nome, Código, Data, etc.) em tempo de execução.
- **Integridade Referencial:**
- Validação de chaves estrangeiras (não insere Pet se o Dono ou Tipo não existirem).
- **Exclusão em Cascata:** Ao deletar um Dono, seus Pets são removidos. Ao deletar um Tipo, todos os Pets daquele tipo são removidos.

- **Persistência de Dados:** Armazenamento em arquivos binários (`.bin`) com suporte a "Soft Delete" (Exclusão Lógica) e reutilização de IDs.

## 🛠️ Estruturas de Dados Utilizadas

O projeto demonstra o uso prático e integrado das seguintes estruturas:

1. **Filas Dinâmicas (Queues):** Para armazenar e organizar a ordem de chegada dos comandos antes do processamento.
2. **Listas Duplamente Encadeadas:** Para manutenção dos dados em memória RAM durante a execução, permitindo navegação bidirecional e remoção eficiente.
3. **Árvores Binárias (Binary Trees):** Construídas dinamicamente apenas durante a execução do comando `SELECT ... ORDER BY` para garantir a ordenação dos dados sem alterar a estrutura original da lista.
4. **Manipulação de Arquivos Binários:** Leitura e escrita direta em disco (`fread`/`fwrite`) para persistência dos dados.

## 📂 Estrutura do Projeto

- **`main.c`**: Ponto de entrada, menu interativo e orquestração do sistema.
- **`validador.c`**: Parser robusto responsável por ler o arquivo de texto, validar a sintaxe SQL e distribuir os comandos nas filas.
- **`comando.c`**: Gerenciamento da estrutura de Filas genérica.
- **`pessoa.c`, `pet.c`, `tipoPet.c**`: Implementação das Listas Encadeadas, Árvores de ordenação e regras de negócio de cada entidade.
- **`ArquivosBinarios/`**: Pasta obrigatória onde os bancos de dados (`.bin`) são salvos.

## ⚙️ Como Compilar e Executar

### Pré-requisitos

- Compilador C (GCC recomendado).
- Sistema Operacional Windows (devido ao uso de `system("cls")` e `system("pause")`) ou adaptação simples para Linux.

### Passo a Passo

1. **Criar a pasta de armazenamento:**
   Certifique-se de que existe uma pasta chamada `ArquivosBinarios` na raiz do projeto. O sistema precisa dela para salvar os dados.
2. **Compilar:**
   Abra o terminal na pasta do projeto e execute:

```bash
gcc main.c validador.c comando.c pessoa.c pet.c tipoPet.c -o petshop.exe

```

3. **Executar:**

```bash
./petshop.exe

```

## 📝 Como Usar (Sintaxe dos Comandos)

O sistema lê os comandos do arquivo `comandos.txt`. Você pode editar este arquivo e inserir comandos no seguinte formato:

### Inserção

```sql
INSERT INTO tipo_pet (codigo, descricao) VALUES (1, 'Cachorro');
INSERT INTO pessoa (codigo, nome, fone) VALUES (10, 'Joao Silva', '9999-1234');
INSERT INTO pet (codigo, dono, tipo, nome) VALUES (100, 10, 1, 'Rex');

```

### Atualização

```sql
UPDATE pessoa SET nome = 'Joao da Silva Sauro' WHERE codigo = 10;
UPDATE pet SET dono = 20 WHERE nome = 'Rex';

```

### Remoção (Suporta Cascata)

```sql
DELETE FROM pessoa WHERE codigo = 10;
-- Isso removerá o Joao e automaticamente removerá o Rex (Cascata)

```

### Consulta e Ordenação

```sql
SELECT * FROM pessoa ORDER BY nome;
SELECT * FROM pet WHERE tipo = 1 ORDER BY codigo_pes;

```

## 🛡️ Robustez e Tratamento de Erros

O sistema conta com um validador avançado que:

- Ignora diferenças entre maiúsculas e minúsculas (**Case Insensitive**).
- Trata espaços extras e formatação irregular.
- Bloqueia inserções com chaves estrangeiras inválidas.
- Impede a corrupção de memória limpando as estruturas corretamente ao encerrar.

---

**Autores:** Pedro Henrique Mauricio Alves e Josué Levi Oliveira Lima
**Curso:** Engenharia de Computação
**Disciplina:** Estrutura de Dados
