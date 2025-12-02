As pastas **/bin** e **/obj** são criadas automaticamente ao rodar o `make` pela primeira vez.

---

## Pré-requisitos

Antes de compilar, verifique se possui:

- GCC – compilador C  
- Make – ferramenta de build

Verifique com:

```bash
gcc --version
make --version

## Compilar (No nível de diretório do Makefile)

```bash
make

OBS: Rode mais de uma vez se na primeira não compilar. Isto acontece porque as pastas bin e obj podem não ter sido criadas.

## Rodar (No nível de diretório do Makefile)

```bash
make run