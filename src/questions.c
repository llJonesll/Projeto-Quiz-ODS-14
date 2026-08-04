/**
 * @file questions.c
 * @author Grupo 1
 * @brief Implementação do módulo do Banco de Questões.
 * @version 1.0
 * @copyright Copyright (c) 2025
 */

#include "raylib/questions.h"
#include <stdlib.h> // Para rand() e srand()

//---------------------------------------------
// Definições e Constantes (Privadas ao Módulo)
//---------------------------------------------
#define TOTAL_QUESTIONS 40
#define NUM_EASY 8
#define NUM_MEDIUM 8
#define NUM_HARD 4

//---------------------------------------------
// Variáveis Estáticas (Privadas ao Módulo)
//---------------------------------------------
static Question questions[TOTAL_QUESTIONS];
static int easyQuestionIndices[TOTAL_QUESTIONS];
static int mediumQuestionIndices[TOTAL_QUESTIONS];
static int hardQuestionIndices[TOTAL_QUESTIONS];
static int easyCount = 0, mediumCount = 0, hardCount = 0;

//---------------------------------------------
// Funções Privadas
//---------------------------------------------
static void ShuffleIntArray(int *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

//---------------------------------------------
// Implementação das Funções Públicas
//---------------------------------------------

void InitializeQuestions(void) {
    // Fáceis - 10 Pontos
    questions[0] = (Question){"Qual e a principal meta da ODS 14 (Vida na Agua) para garantir a sustentabilidade dos mares?", {"Proteger ecossistemas marinhos e combater a poluicao e a sobrepesca", "Proibir qualquer tipo de atividade economica ou turismo nas praias", "Aumentar a extracao de petroleo em aguas profundas", "Incentivar a expansao urbana sobre as regioes de manguezal"}, 0, EASY, 10};
    questions[1] = (Question){"Qual material sintetico de uso cotidiano leva centenas de anos para se decompor e e o maior poluidor dos mares?", {"Vidro", "Plastico", "Aluminio", "Papel biologico"}, 1, EASY, 10};
    questions[2] = (Question){"O aumento do aquecimento global impacta os corais marinhos provocando um fenomeno visual e biologico chamado:", {"Eutrofizacao", "Salinizacao", "Branqueamento", "Evaporacao"}, 2, EASY, 10};
    questions[3] = (Question){"Muitas tartarugas marinhas morrem ao ingerir sacolas plasticas no mar porque:", {"O cheiro do plastico atrai os repteis", "Elas usam o plastico para afundar mais rapido", "O plastico substitui a necessidade de agua doce", "Elas confundem o lixo com aguas-vivas, sua principal fonte de alimento"}, 3, EASY, 10};
    questions[4] = (Question){"Qual a porcentagem aproximada da superficie do nosso planeta coberta por agua salgada?", {"Cerca de 70%", "Cerca de 50%", "Cerca de 90%", "Cerca de 30%"}, 0, EASY, 10};
    questions[5] = (Question){"Qual vegetacao costeira presente no Brasil e fundamental para evitar a erosao da praia e abrigar caranguejos?", {"Restinga rochosa", "Manguezal", "Caatinga", "Campos sulinos"}, 1, EASY, 10};
    questions[6] = (Question){"De onde vem a maior parte do oxigenio molecular que os seres humanos e outros animais respiram?", {"Das florestas tropicais terrestres", "Da evaporacao das aguas dos rios", "Do fitoplancton e das algas presentes nos oceanos", "Do gas liberado durante erupcoes vulcanicas"}, 2, EASY, 10};
    questions[7] = (Question){"A retirada de peixes do mar em velocidade superior a capacidade de reproducao da especie e chamada de:", {"Pesca artesanal", "Aquicultura", "Piscicultura", "Sobrepesca"}, 3, EASY, 10};
    questions[8] = (Question){"Acidentes com navios petroleiros geram o 'oleo negro' nas praias. Qual o principal impacto imediato desse vazamento?", {"Bloqueio da luz solar e sufocamento da fauna marinha e aves", "Aumento do teor de sal na agua da praia", "Diminuicao da temperatura da agua do mar", "Crescimento acelerado de arvores na costa"}, 0, EASY, 10};
    questions[9] = (Question){"Para que servem as Reservas Extrativistas (RESEX) e Areas de Protecao Ambiental no litoral?", {"Liberar o descarte de lixo industrial longe das cidades", "Garantir a preservacao da natureza e o sustento de comunidades tradicionais", "Construir resorts e hoteis de luxo privativos", "Reservar espaco para testes de embarcacoes militares"}, 1, EASY, 10};
    
    // Médias - 25 Pontos
    questions[10] = (Question){"A queimada de combustiveis fosseis gera CO2, que e absorvido pelos oceanos. O resultado quimico disso no mar e:", {"O aumento do teor de oxigenio na agua", "O aumento do sal presente na agua", "A acidificacao da agua, prejudicando animais de concha e corais", "O congelamento rapido das zonas tropicais"}, 2, MEDIUM, 25};
    questions[11] = (Question){"O que sao os 'microplasticos' e por que eles sao um perigo na nossa alimentacao?", {"Pequenos robos usados para limpar a agua que entram nos peixes", "Embalagens biodegradaveis que nao causam danos ao meio ambiente", "Pedacos de plastico invisiveis a olho nu que nao fazem mal a saude", "Particulas minusculas de plastico engolidas por peixes que chegam ao nosso prato"}, 3, MEDIUM, 25};
    questions[12] = (Question){"Quando redes de pesca sao abandonadas no mar por barcos industriais, elas continuam matando animais sozinhos. Isso e a:", {"Pesca fantasma", "Pesca artesanal", "Predacao natural", "Migracao marinha"}, 0, MEDIUM, 25};
    questions[13] = (Question){"O esgoto sem tratamento jogado no mar leva muitos nutrientes para a agua, causando a proliferacao de algas e falta de ar. Isso e:", {"Salinizacao", "Eutrofizacao", "Evaporacao", "Condensacao"}, 1, MEDIUM, 25};
    questions[14] = (Question){"Como a vegetacao de mangue ajuda a combater o avanco da agua sobre as casas em cidades litoraneas?", {"Suas raizes trancadas amortecem a forca das ondas e prendem a areia", "Elas absorvem toda a agua da mare impedindo que a agua suba", "Suas folhas criam uma barreira contra o vento forte do oceano", "Elas transformam a agua salgada em agua doce instantaneamente"}, 0, MEDIUM, 25};
    questions[15] = (Question){"Por que a introducao do peixe-leao (nativo da Asia) nas praias do Brasil e um problema ambiental grave?", {"Porque ele e um peixe herbivoro que come toda a areia da praia", "Porque ele nao tem predadores naturais aqui e devora os peixes locais", "Porque ele ataca apenas banhistas nas praias de agua doce", "Porque ele produz plastico dentro do seu organismo"}, 1, MEDIUM, 25};
    questions[16] = (Question){"Navios gigantes e motores subaquaticos geram barulho no mar. Como isso afeta animais como baleias e golfinhos?", {"Impede que eles consigam enxergar o fundo do mar", "Faz com que os peixes percam o escamar natural", "Desorienta sua navegacao e comunicacao, que dependem do som", "Aumenta o peso dos animais impedindo que eles flutuem"}, 2, MEDIUM, 25};
    questions[17] = (Question){"O termo ambiental 'Carbono Azul' refere-se ao papel dos ecossistemas costeiros em:", {"Poluir o ar com fumaça azul vinda das algas", "Extrair gas natural do fundo do solo marinho", "Refletir a luz do Sol fazendo o mar parecer azul", "Absorver e guardar o carbono da atmosfera nas plantas e no solo marinho"}, 3, MEDIUM, 25};
    questions[18] = (Question){"A maior parte do lixo que chega aos oceanos nao vem de navios. De onde vem esse lixo?", {"De meteoritos que caem do espaco", "De cidades e rios no continente devido ao descarte incorreto", "De vulcoes subaquaticos em erupcao", "Da evaporacao natural da agua do mar"}, 1, MEDIUM, 25};
    questions[19] = (Question){"O derretimento do gelo nos polos afeta as cidades praianas do mundo todo porque causa:", {"A diminuicao do calor nas regioes de praia", "O aumento das tempestades de neve no Nordeste", "A elevação do nivel do mar e o alagamento de regioes costeiras", "A transformacao de agua salgada em agua mineral"}, 2, MEDIUM, 25};

    // Difíceis - 50 Pontos
    questions[20] = (Question){"As correntes maritimas acumulam toneladas de lixo plastico flutuante em pontos fixos do oceano. O maior deles e o:", {"Giro de Lixo do Pacifico Norte", "Triangulo das Bermudas", "Canal da Mancha", "Abismo das Marianas"}, 2, HARD, 50};
    questions[21] = (Question){"O Brasil tem direito exclusivo de explorar os recursos naturais do mar ate 200 milhas da costa. Esse territorio e o(a):", {"Mar Territorial de 12 milhas", "Fronteira Terrestre", "Espaco Aereo Internacional", "Amazonia Azul (Zona Economica Exclusiva)"}, 3, HARD, 50};
    questions[22] = (Question){"Para garantir que uma especie de peixe nao entre em extincao, os biologos calculam a cota maxima sustentavel de pesca baseada em:", {"Na quantidade de peixes que nasce e cresce sem diminuir a populacao", "No lucro financeiro que o pescador precisa ter no mes", "Tamanho do tanque de combustivel dos barcos de pesca", "Na quantidade de barcos registrados na capitania dos portos"}, 0, HARD, 50};
    questions[23] = (Question){"O fitoplancton marinho e a base da teia alimentar do mar. O que aconteceria se ele desaparecesse?", {"Os tubaroes passariam a comer apenas plantas da terra", "Faltaria oxigenio no planeta e a maioria das especies marinhas morreria", "A agua do mar ficaria potavel e propria para beber", "Os oceanos secariam completamente em poucas semanas"}, 1, HARD, 50};
    questions[24] = (Question){"Zonas mortas no mar sao areas com 'hipoxia'. O que significa ter um ambiente em hipoxia?", {"Uma regiao onde a agua e extremamente quente", "Um local com niveis de oxigenio tao baixos que a vida animal nao resiste", "Uma area com excesso de sal que impede o nado de peixes", "Um ponto do mar onde a luz do Sol e muito forte"}, 1, HARD, 50};
    questions[25] = (Question){"Qual gas poluente emitido por carros e indústrias e o maior responsavel por alterar a quimica da agua do mar?", {"Gas Oxigenio (O2)", "Gas Nitrogenio (N2)", "Dioxido de Carbono / Gas Carbonico (CO2)", "Gas Helio (He)"}, 2, HARD, 50};
    questions[26] = (Question){"A ONU declarou a 'Decada da Ciencia Oceanica (2021-2030)' com o objetivo principal de:", {"Construir mais cidades artificiais dentro do mar", "Substituir a agua dos oceanos por agua de reuso", "Extrair todo o ouro presente no fundo dos mares", "Unir a ciencia e a sociedade para salvar e recuperar os oceanos"}, 3, HARD, 50};
    questions[27] = (Question){"Metais pesados como o mercurio lancados no mar vao se acumulando em maior quantidade nos peixes maiores. Isso e a:", {"Biomagnificacao (ou bioacumulacao trofica)", "Fotossintese marinha", "Erosao marinha", "Evapotranspiracao"}, 0, HARD, 50};
    questions[28] = (Question){"Uma das metas da ODS 14 e fazer com que os governos parem de dar dinheiro (subsidios) para:", {"Barcos que usam equipamentos de seguranca modernos", "Praticas de pesca ilegal ou descontrolada que destroem a fauna", "Projetos de pesquisa cientifica em universidades públicas", "Acoes de limpeza de lixo nas praias urbanas"}, 1, HARD, 50};
    questions[29] = (Question){"Quando um mergulhador desce no oceano, ele sente uma queda brusca de temperatura em uma certa profundidade. Essa camada e a:", {"Atmosfera", "Litosfera", "Termoclina", "Biosfera"}, 2, HARD, 50};
    
    // Novas Perguntas
    questions[30] = (Question){"A baleia-azul e considerada uma referencia no reino animal por ser:", {"O animal mais pesado e longo de toda a historia da Terra", "O unico ser vivo que nao precisa de alimento para viver", "Um peixe perigoso que ataca grandes embarcacoes", "Uma especie de reptil gigante que vive no mar"}, 2, EASY, 10};
    questions[31] = (Question){"Enquanto os humanos usam pulmoes para tirar oxigenio do ar, os peixes usam branquias (guelras) para:", {"Filtrar o oxigenio dissolvido na propria agua", "Armazenar agua doce para viagens longas", "Expulsar o excesso de plastico do corpo", "Esfriar a temperatura do mar ao seu redor"}, 1, EASY, 10};
    questions[32] = (Question){"Qual o nome dado ao agrupamento de centenas de peixes da mesma especie nadando juntos para se proteger?", {"Alcateia", "Cardume", "Enxame", "Rebanho"}, 1, EASY, 10};
    questions[33] = (Question){"A subida e a descida da mare nas praias ao longo do dia acontecem principalmente por causa da:", {"Forca dos ventos que vem do continente", "Velocidade de rotacao dos navios no mar", "Temperatura da areia da praia durante o dia", "Atraçao da gravidade que a Lua e o Sol exercem sobre a Terra"}, 3, EASY, 10};
    questions[34] = (Question){"Alguns seres marinhos que vivem no fundo do mar conseguem emitir luz propria no escuro. Esse fenomeno e a:", {"Fotossintese", "Bioluminescencia", "Reflexao solar", "Transpiracao"}, 1, MEDIUM, 25};
    questions[35] = (Question){"A Grande Barreira de Corais e visivel ate do espaco. Em qual pais fica essa gigante estrutura natural?", {"Brasil", "Estados Unidos", "Japao", "Australia"}, 3, MEDIUM, 25};
    questions[36] = (Question){"O ciclo da agua e essencial para a vida. A passagem da agua do mar para a atmosfera em forma de vapor e a:", {"Sublimacao", "Evaporacao", "Precipitacao", "Infiltracao"}, 1, MEDIUM, 25};
    questions[37] = (Question){"O ponto mais fundo ja explorado pelos seres humanos nos oceanos fica no Pacifico e se chama:", {"Fossa das Marianas", "Trincheira do Atlantico", "Caverna de Iceberg", "Plataforma de Santos"}, 0, HARD, 50};
    questions[38] = (Question){"Redes industriais costumam capturar tartarugas e golfinhos sem querer. Na pesca, esse problema e chamado de:", {"Captura esportiva", "Pesca limpa", "Captura acidental (ou Bycatch)", "Descarte ecologico"}, 2, HARD, 50};
    questions[39] = (Question){"O acordo internacional chamado MARPOL serve para proibir que navios de todo o mundo:", {"Viajem durante o periodo do inverno", "Transportem alimentos entre os paises", "Usem radares durante a navegacao noturna", "Joguem lixo, esgoto e oleo diretamente nas aguas do mar"}, 3, HARD, 50};
    
    for (int i = 0; i < TOTAL_QUESTIONS; i++) {
        switch (questions[i].difficulty) {
            case EASY:   easyQuestionIndices[easyCount++] = i;   break;
            case MEDIUM: mediumQuestionIndices[mediumCount++] = i; break;
            case HARD:   hardQuestionIndices[hardCount++] = i;   break;
        }
    }
}

void SelectAndShuffleQuizQuestions(int *questionOrder) {
    ShuffleIntArray(easyQuestionIndices, easyCount);
    ShuffleIntArray(mediumQuestionIndices, mediumCount);
    ShuffleIntArray(hardQuestionIndices, hardCount);
    
    int currentQuizIndex = 0;
    for (int i = 0; i < NUM_EASY && i < easyCount; i++) questionOrder[currentQuizIndex++] = easyQuestionIndices[i];
    for (int i = 0; i < NUM_MEDIUM && i < mediumCount; i++) questionOrder[currentQuizIndex++] = mediumQuestionIndices[i];
    for (int i = 0; i < NUM_HARD && i < hardCount; i++) questionOrder[currentQuizIndex++] = hardQuestionIndices[i];
    
    ShuffleIntArray(questionOrder, QUIZ_QUESTION_COUNT);
}

Question GetQuestionFromOrder(int orderIndex) {
    return questions[orderIndex];
}