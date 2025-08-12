#include <stdio.h>
#include <stdlib.h>
#include <string.h>

  void testeone(int vetor[], int n){
   for(int i = n-1; i >=0 ; i--){
     printf("\n %d", vetor[i]);
     
   }
     
  }

int main(){
   int n;
  printf("Tell me the size of the vector: ");
   scanf("%d", &n);
  int v[n];
  for(int i = 0; i < n; i++){
    printf("Tell me the value of the vector int the %d position: ", i+1);
   scanf("%d", &v[i]);
  }
  printf("The vector in reverse order is: ");
  testeone(v, n);
  

   return 0;
}

/* Eu não fiz nada que esta abaixo*/







#include <ctype.h>

#define MAX_LINHA_LEN 256
#define MAX_LINHAS 200
#define MAX_ESCOPOS 10
#define MAX_VARS_POR_ESCOPO 100
#define MAX_FUNCS 20
#define MAX_PARAMS 10

typedef enum {TIPO_INT, TIPO_FLOAT, TIPO_STRING} TipoVar;

typedef struct {
    char nome[32];
    TipoVar tipo;
    union {
        int valor_int;
        float valor_float;
        char valor_str[128];
    };
} Variavel;

typedef struct {
    Variavel vars[MAX_VARS_POR_ESCOPO];
    int num_vars;
} Escopo;

typedef struct {
    char nome[32];
    char parametros[MAX_PARAMS][32];
    int num_parametros;
    char corpo[MAX_LINHAS][MAX_LINHA_LEN];
    int num_linhas;
} Funcao;

// Pilha de escopos
Escopo pilha_escopos[MAX_ESCOPOS];
int topo_escopo = -1;

// Lista de funções
Funcao funcoes[MAX_FUNCS];
int num_funcoes = 0;

// Protótipos
void abre_escopo();
void fecha_escopo();
Variavel* get_var(const char *nome);
void set_var(const char *nome, TipoVar tipo, void *valor);
int interpreta_bloco(char linhas[][MAX_LINHA_LEN], int inicio, int fim);
int avalia_expressao(char *expr, TipoVar *tipo_ret, void *ret);

// Funções para manipular escopo e variáveis

void abre_escopo() {
    if (topo_escopo < MAX_ESCOPOS -1) {
        topo_escopo++;
        pilha_escopos[topo_escopo].num_vars = 0;
    } else {
        printf("Erro: pilha de escopos cheia\n");
    }
}

void fecha_escopo() {
    if (topo_escopo >= 0) topo_escopo--;
    else printf("Erro: nenhum escopo para fechar\n");
}

int procura_var_escopo(const char *nome, int *escopo_idx) {
    for (int i = topo_escopo; i >= 0; i--) {
        for (int j = 0; j < pilha_escopos[i].num_vars; j++) {
            if (strcmp(pilha_escopos[i].vars[j].nome, nome) == 0) {
                *escopo_idx = i;
                return j;
            }
        }
    }
    return -1;
}

Variavel* get_var(const char *nome) {
    int escopo_idx, var_idx;
    var_idx = procura_var_escopo(nome, &escopo_idx);
    if (var_idx >= 0) return &pilha_escopos[escopo_idx].vars[var_idx];
    return NULL;
}

void set_var(const char *nome, TipoVar tipo, void *valor) {
    int escopo_idx, var_idx;
    var_idx = procura_var_escopo(nome, &escopo_idx);
    if (var_idx >= 0) {
        Variavel *v = &pilha_escopos[escopo_idx].vars[var_idx];
        v->tipo = tipo;
        if (tipo == TIPO_INT) v->valor_int = *(int*)valor;
        else if (tipo == TIPO_FLOAT) v->valor_float = *(float*)valor;
        else if (tipo == TIPO_STRING) strcpy(v->valor_str, (char*)valor);
    } else {
        if (topo_escopo < 0) {
            printf("Erro: nenhum escopo aberto\n");
            return;
        }
        Escopo *e = &pilha_escopos[topo_escopo];
        if (e->num_vars < MAX_VARS_POR_ESCOPO) {
            Variavel *v = &e->vars[e->num_vars++];
            strcpy(v->nome, nome);
            v->tipo = tipo;
            if (tipo == TIPO_INT) v->valor_int = *(int*)valor;
            else if (tipo == TIPO_FLOAT) v->valor_float = *(float*)valor;
            else if (tipo == TIPO_STRING) strcpy(v->valor_str, (char*)valor);
        } else {
            printf("Erro: limite de variáveis no escopo atingido\n");
        }
    }
}

// Função simples para imprimir variável
void imprime_variavel(Variavel *v) {
    if (!v) {
        printf("NULL\n");
        return;
    }
    if (v->tipo == TIPO_INT) printf("%d\n", v->valor_int);
    else if (v->tipo == TIPO_FLOAT) printf("%f\n", v->valor_float);
    else if (v->tipo == TIPO_STRING) printf("%s\n", v->valor_str);
}

// Avaliar expressões básicas com suporte a int e variáveis int (exemplo simplificado)
int avalia_expressao(char *expr, TipoVar *tipo_ret, void *ret) {
    // Para simplificar, vamos considerar expressões que sejam:
    // - número inteiro (ex: 5)
    // - nome de variável int
    // - operação simples: a + b (inteiros)
    // Este é um exemplo básico, pode ser expandido depois.

    char *p = expr;
    while (isspace(*p)) p++;

    // Se número inteiro
    if (isdigit(*p) || (*p == '-' && isdigit(*(p+1)))) {
        int val = atoi(p);
        *(int*)ret = val;
        *tipo_ret = TIPO_INT;
        return 1;
    }

    // Se variável int
    Variavel *v = get_var(p);
    if (v && v->tipo == TIPO_INT) {
        *(int*)ret = v->valor_int;
        *tipo_ret = TIPO_INT;
        return 1;
    }

    // Para expressões com + (exemplo muito simples)
    char op;
    char left[128], right[128];
    if (sscanf(expr, "%127[^+ ] %c %127s", left, &op, right) == 3) {
        TipoVar tl, tr;
        int vl, vr;
        if (avalia_expressao(left, &tl, &vl) && avalia_expressao(right, &tr, &vr)) {
            if (op == '+') {
                *(int*)ret = vl + vr;
                *tipo_ret = TIPO_INT;
                return 1;
            } else if (op == '-') {
                *(int*)ret = vl - vr;
                *tipo_ret = TIPO_INT;
                return 1;
            } else if (op == '*') {
                *(int*)ret = vl * vr;
                *tipo_ret = TIPO_INT;
                return 1;
            } else if (op == '/') {
                if (vr == 0) {
                    printf("Erro: divisão por zero\n");
                    return 0;
                }
                *(int*)ret = vl / vr;
                *tipo_ret = TIPO_INT;
                return 1;
            }
        }
    }

    printf("Erro: expressão inválida: %s\n", expr);
    return 0;
}

// Interpreta bloco de linhas entre inicio e fim, retorna 1 sucesso, 0 erro
int interpreta_bloco(char linhas[][MAX_LINHA_LEN], int inicio, int fim) {
    for (int i = inicio; i <= fim; i++) {
        char *linha = linhas[i];
        while (*linha && isspace(*linha)) linha++; // trim left

        if (strncmp(linha, "VAR ", 4) == 0) {
            // VAR x = expr
            char nome[32];
            char expr[128];
            if (sscanf(linha+4, "%31s = %127[^\n]", nome, expr) == 2) {
                TipoVar tipo;
                int val;
                if (avalia_expressao(expr, &tipo, &val)) {
                    set_var(nome, tipo, &val);
                } else {
                    printf("Erro na expressão da VAR.\n");
                    return 0;
                }
            } else {
                printf("Sintaxe inválida para VAR.\n");
                return 0;
            }
        }
        else if (strncmp(linha, "IMPRIMIR ", 8) == 0) {
            // IMPRIMIR expr ou IMPRIMIR "string"
            char *arg = linha + 8;
            while (*arg && isspace(*arg)) arg++;

            if (*arg == '"') {
                // string
                arg++;
                char *fim_str = strchr(arg, '"');
                if (!fim_str) {
                    printf("Erro: string não fechada\n");
                    return 0;
                }
                *fim_str = '\0';
                printf("%s\n", arg);
            } else {
                // expressão
                TipoVar tipo;
                int val;
                if (avalia_expressao(arg, &tipo, &val)) {
                    printf("%d\n", val);
                } else {
                    printf("Erro na expressão do IMPRIMIR.\n");
                    return 0;
                }
            }
        }
        else if (strncmp(linha, "SE ", 3) == 0) {
            // SE condição ENTAO ... SENAO ... FIMSE
            char *p = linha + 3;
            char cond[128];
            char *entao = strstr(p, "ENTAO");
            if (!entao) {
                printf("Erro: falta ENTAO\n");
                return 0;
            }
            int len_cond = entao - p;
            strncpy(cond, p, len_cond);
            cond[len_cond] = '\0';

            // Avaliar condição simplificada (ex: x > 5)
            char var[32], op[3];
            int val;
            int num;

            if (sscanf(cond, "%31s %2s %d", var, op, &num) != 3) {
                printf("Erro: condição inválida: %s\n", cond);
                return 0;
            }

            Variavel *vv = get_var(var);
            if (!vv) {
                printf("Erro: variável não definida na condição: %s\n", var);
                return 0;
            }
            int cond_verdadeira = 0;
            if (strcmp(op, ">") == 0) cond_verdadeira = (vv->valor_int > num);
            else if (strcmp(op, "<") == 0) cond_verdadeira = (vv->valor_int < num);
            else if (strcmp(op, "==") == 0) cond_verdadeira = (vv->valor_int == num);
            else if (strcmp(op, "!=") == 0) cond_verdadeira = (vv->valor_int != num);
            else if (strcmp(op, ">=") == 0) cond_verdadeira = (vv->valor_int >= num);
            else if (strcmp(op, "<=") == 0) cond_verdadeira = (vv->valor_int <= num);
            else {
                printf("Erro: operador inválido na condição\n");
                return 0;
            }

            // Buscar blocos entre ENTAO, SENAO e FIMSE
            int j = i + 1;
            int fim_se = -1;
            int inicio_senao = -1;
            while (j <= fim) {
                if (strncmp(linhas[j], "SENAO", 5) == 0) {
                    inicio_senao = j + 1;
                }
                if (strncmp(linhas[j], "FIMSE", 5) == 0) {
                    fim_se = j;
                    break;
                }
                j++;
            }
            if (fim_se == -1) {
                printf("Erro: falta FIMSE\n");
                return 0;
            }

            abre_escopo();
            if (cond_verdadeira) {
                interpreta_bloco(linhas, i + 1, (inicio_senao == -1 ? fim_se - 1 : inicio_senao - 2));
            } else if (inicio_senao != -1) {
                interpreta_bloco(linhas, inicio_senao, fim_se - 1);
            }
            fecha_escopo();

            i = fim_se;
        }
        else if (strncmp(linha, "ENQUANTO ", 9) == 0) {
            // ENQUANTO condição ... FIMENQUANTO
            char cond[128];
            strcpy(cond, linha + 9);

            int j = i + 1;
            int fim_enquanto = -1;
            while (j <= fim) {
                if (strncmp(linhas[j], "FIMENQUANTO", 11) == 0) {
                    fim_enquanto = j;
                    break;
                }
                j++;
            }
            if (fim_enquanto == -1) {
                printf("Erro: falta FIMENQUANTO\n");
                return 0;
            }

            // Loop
            while (1) {
                // Avaliar condição
                char var[32], op[3];
                int num;

                if (sscanf(cond, "%31s %2s %d", var, op, &num) != 3) {
                    printf("Erro: condição inválida: %s\n", cond);
                    return 0;
                }
                Variavel *vv = get_var(var);
                if (!vv) {
                    printf("Erro: variável não definida na condição: %s\n", var);
                    return 0;
                }
                int cond_verdadeira = 0;
                if (strcmp(op, ">") == 0) cond_verdadeira = (vv->valor_int > num);
                else if (strcmp(op, "<") == 0) cond_verdadeira = (vv->valor_int < num);
                else if (strcmp(op, "==") == 0) cond_verdadeira = (vv->valor_int == num);
                else if (strcmp(op, "!=") == 0) cond_verdadeira = (vv->valor_int != num);
                else if (strcmp(op, ">=") == 0) cond_verdadeira = (vv->valor_int >= num);
                else if (strcmp(op, "<=") == 0) cond_verdadeira = (vv->valor_int <= num);
                else {
                    printf("Erro: operador inválido na condição\n");
                    return 0;
                }
                if (!cond_verdadeira) break;

                abre_escopo();
                if (!interpreta_bloco(linhas, i + 1, fim_enquanto - 1)) {
                    fecha_escopo();
                    return 0;
                }
                fecha_escopo();
            }
            i = fim_enquanto;
        }
        else if (strncmp(linha, "FUNCAO ", 7) == 0) {
            if (num_funcoes >= MAX_FUNCS) {
                printf("Erro: limite de funções atingido\n");
                return 0;
            }
            Funcao *f = &funcoes[num_funcoes];

            // Extrair nome e params
            char *p = linha + 7;
            char *ab = strchr(p, '(');
            if (!ab) {
                printf("Erro: sintaxe FUNCAO\n");
                return 0;
            }
            int nome_len = ab - p;
            strncpy(f->nome, p, nome_len);
            f->nome[nome_len] = '\0';

            char *fb = strchr(ab, ')');
            if (!fb) {
                printf("Erro: sintaxe FUNCAO\n");
                return 0;
            }

            char params[128];
            int plen = fb - ab - 1;
            strncpy(params, ab + 1, plen);
            params[plen] = '\0';

            f->num_parametros = 0;
            char *tok = strtok(params, ",");
            while (tok && f->num_parametros < MAX_PARAMS) {
                while (*tok == ' ') tok++;
                char *end = tok + strlen(tok) - 1;
                while (end > tok && *end == ' ') { *end = '\0'; end--; }
                strcpy(f->parametros[f->num_parametros++], tok);
                tok = strtok(NULL, ",");
            }

            int k = i + 1;
            f->num_linhas = 0;
          // Ler corpo da função até FIMFUNCAO
            while (k <= fim && strcmp(linhas[k], "FIMFUNCAO") != 0) {
                strcpy(f->corpo[f->num_linhas++], linhas[k]);
                k++;
            }
            if (k > fim) {
                printf("Erro: falta FIMFUNCAO\n");
                return 0;
            }
            num_funcoes++;
            i = k; // pula linhas da função
        }
        else if (strncmp(linha, "CHAMAR ", 7) == 0) {
            // CHAMAR funcao(param1, param2)
            char *p = linha + 7;
            char nome[32];
            char args[128];

            char *ab = strchr(p, '(');
            char *fb = strchr(p, ')');
            if (!ab || !fb || fb < ab) {
                printf("Erro: sintaxe CHAMAR\n");
                return 0;
            }
            int nome_len = ab - p;
            strncpy(nome, p, nome_len);
            nome[nome_len] = '\0';

            int args_len = fb - ab - 1;
            strncpy(args, ab + 1, args_len);
            args[args_len] = '\0';

            Funcao *f = NULL;
            for (int x = 0; x < num_funcoes; x++) {
                if (strcmp(funcoes[x].nome, nome) == 0) {
                    f = &funcoes[x];
                    break;
                }
            }
            if (!f) {
                printf("Erro: função %s não encontrada\n", nome);
                return 0;
            }

            // Divide argumentos
            char *argv[MAX_PARAMS];
            int argc = 0;
            char *tok = strtok(args, ",");
            while (tok && argc < MAX_PARAMS) {
                while (*tok == ' ') tok++;
                char *end = tok + strlen(tok) - 1;
                while (end > tok && *end == ' ') { *end = '\0'; end--; }
                argv[argc++] = tok;
                tok = strtok(NULL, ",");
            }
            if (argc != f->num_parametros) {
                printf("Erro: número errado de argumentos na chamada\n");
                return 0;
            }

            // Salvar variáveis atuais para restaurar depois (escopo simples)
            Escopo backup_escopo = pilha_escopos[topo_escopo];
            int backup_topo = topo_escopo;

            abre_escopo();
            // Definir parâmetros como variáveis locais
            for (int i = 0; i < argc; i++) {
                TipoVar tipo;
                int val;
                if (avalia_expressao(argv[i], &tipo, &val)) {
                    set_var(f->parametros[i], tipo, &val);
                } else {
                    printf("Erro: argumento inválido na chamada\n");
                    fecha_escopo();
                    return 0;
                }
            }

            if (!interpreta_bloco(f->corpo, 0, f->num_linhas - 1)) {
                fecha_escopo();
                return 0;
            }
            fecha_escopo();

            // Restaurar escopo anterior (simplificado)
            pilha_escopos[topo_escopo] = backup_escopo;
            topo_escopo = backup_topo;
        }
        else {
            if (linha[0] != '\0') {
                printf("Comando desconhecido: %s\n", linha);
                return 0;
            }
        }
    }
    return 1;
}

int main() {
    char programa[MAX_LINHAS][MAX_LINHA_LEN];
    int num_linhas = 0;

    printf("Digite seu programa (digite 'FIM' para terminar):\n");

    while (num_linhas < MAX_LINHAS) {
        if (!fgets(programa[num_linhas], MAX_LINHA_LEN, stdin)) break;
        programa[num_linhas][strcspn(programa[num_linhas], "\n")] = 0; // remove \n
        if (strcmp(programa[num_linhas], "FIM") == 0) break;
        num_linhas++;
    }

    abre_escopo();  // escopo global

    if (!interpreta_bloco(programa, 0, num_linhas - 1)) {
        printf("Erro na interpretação do programa.\n");
    }

    fecha_escopo();

    return 0;
}