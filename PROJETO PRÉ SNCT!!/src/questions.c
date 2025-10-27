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
    questions[0] = (Question){"Qual o principal objetivo da ODS 14: Vida na Agua?", {"Conservar e usar de forma sustentavel os oceanos e mares", "Aumentar a producao de peixes para alimentacao", "Promover o turismo em todas as areas costeiras", "Incentivar a extracao de petroleo no fundo do mar"}, 0, EASY, 10};
    questions[1] = (Question){"Qual material representa a maior parte do lixo encontrado nos oceanos?", {"Vidro", "Plastico", "Metal", "Papel"}, 1, EASY, 10};
    questions[2] = (Question){"O que causa o 'branqueamento' dos corais?", {"Excesso de peixes na regiao", "Sombras de barcos passando", "Aumento da temperatura da agua", "Falta de sal no mar"}, 2, EASY, 10};
    questions[3] = (Question){"Por que as tartarugas marinhas frequentemente comem sacolas plasticas?", {"Porque sao coloridas e chamativas", "Porque gostam do sabor do plastico", "Porque sentem o cheiro de comida nelas", "Porque confundem com aguas-vivas, seu alimento"}, 3, EASY, 10};
    questions[4] = (Question){"Qual a porcentagem aproximada da superficie da Terra que e coberta por oceanos?", {"Cerca de 70%", "Cerca de 50%", "Cerca de 90%", "Cerca de 30%"}, 0, EASY, 10};
    questions[5] = (Question){"Qual destes ecossistemas costeiros e um 'bercario' para muitas especies marinhas e protege a costa contra erosoes?", {"Praias rochosas", "Manguezais", "Dunas de areia", "Falesias"}, 1, EASY, 10};
    questions[6] = (Question){"De onde vem a maior parte do oxigenio que respiramos?", {"Das arvores da Amazonia", "De grandes plantacoes de soja", "Das algas e fitoplancton marinho", "Liberado por atividade vulcanica"}, 2, EASY, 10};
    questions[7] = (Question){"O que e a 'sobrepesca'?", {"Pescar apenas peixes muito grandes", "Usar barcos de pesca muito rapidos", "Pescar durante a noite", "Capturar peixes mais rapido do que eles conseguem se reproduzir"}, 3, EASY, 10};
    questions[8] = (Question){"O derramamento de qual substancia causa grandes desastres ambientais, matando aves e peixes?", {"Petroleo", "Areia", "Sal", "Açucar"}, 0, EASY, 10};
    questions[9] = (Question){"Qual a principal funcao das Areas Marinhas Protegidas (AMPs)?", {"Servir como rota para navios cargueiros", "Proteger a vida marinha e os habitats", "Ser um local exclusivo para esportes aquaticos", "Area de testes para equipamentos militares"}, 1, EASY, 10};
    
    // Médias - 25 Pontos
    questions[10] = (Question){"Qual fenomeno e causado pela absorcao de CO2 da atmosfera pelos oceanos, prejudicando organismos com conchas?", {"Eutrofizacao", "Salinizacao", "Acidificacao", "Termoclina"}, 2, MEDIUM, 25};
    questions[11] = (Question){"O que sao 'microplasticos'?", {"Pequenos aparelhos eletronicos descartados no mar", "Marcas de plastico biodegradavel", "Organismos que se alimentam de plastico", "Fragmentos de plastico com menos de 5mm"}, 3, MEDIUM, 25};
    questions[12] = (Question){"A 'pesca fantasma' se refere a:", {"Equipamentos de pesca perdidos que continuam a capturar animais", "Pescar em locais assombrados por lendas", "Um tipo de pesca ilegal feita a noite", "Usar iscas que brilham no escuro para atrair peixes"}, 0, MEDIUM, 25};
    questions[13] = (Question){"O que e 'eutrofizacao', um problema comum em areas costeiras?", {"A agua fica mais clara e transparente", "Excesso de nutrientes que causa proliferacao de algas e falta de oxigenio", "Aumento da temperatura media da agua", "Diminuicao da quantidade de sal na agua"}, 1, MEDIUM, 25};
    questions[14] = (Question){"Qual a importancia dos manguezais para o meio ambiente?", {"Servem de barreira de protecao e sao bercarios de especies", "Sao as principais fontes de areia para as praias", "Aumentam a forca das ondas na costa", "Sao locais ideais para a construcao de portos"}, 0, MEDIUM, 25};
    questions[15] = (Question){"Qual destes e um exemplo de especie invasora que causa problemas no Atlantico, incluindo no Brasil?", {"Peixe-palhaco", "Peixe-leao", "Tubarão-martelo", "Cavalo-marinho"}, 1, MEDIUM, 25};
    questions[16] = (Question){"A poluicao sonora nos oceanos, causada por navios e sonares, afeta principalmente qual capacidade das baleias e golfinhos?", {"Sua visao noturna", "Seu olfato para encontrar comida", "Sua comunicacao e localizacao", "Sua capacidade de flutuar"}, 2, MEDIUM, 25};
    questions[17] = (Question){"O que e 'carbono azul' (blue carbon)?", {"Um tipo raro de coral azul", "A poluicao de carbono emitida por navios", "Um combustivel fossil encontrado no fundo do mar", "Carbono capturado e armazenado por ecossistemas marinhos costeiros"}, 3, MEDIUM, 25};
    questions[18] = (Question){"A meta 14.1 da ODS 14 foca em reduzir qual tipo de problema ate 2025?", {"O numero de naufragios de navios", "A poluicao marinha de todos os tipos, especialmente de fontes terrestres", "O barulho gerado por turbinas eolicas no mar", "A quantidade de sal extraida para consumo"}, 1, MEDIUM, 25};
    questions[19] = (Question){"Por que o descongelamento das geleiras e uma ameaca aos oceanos?", {"Aumenta a salinidade da agua", "Cria novas rotas de navegacao", "Eleva o nivel do mar e altera as correntes marinhas", "Diminui a quantidade de peixes"}, 2, MEDIUM, 25};

    // Difíceis - 50 Pontos
    questions[20] = (Question){"O 'Giro do Pacifico Norte' e uma area oceanica conhecida por:", {"Ter as aguas mais quentes do planeta", "Ser a maior rota de migracao de baleias", "Ser o local de uma grande acumulacao de lixo plastico", "Possuir a maior quantidade de vulcoes submarinos ativos"}, 2, HARD, 50};
    questions[21] = (Question){"A Convencao das Nacoes Unidas sobre o Direito do Mar (UNCLOS) define a Zona Economica Exclusiva (ZEE) como uma faixa de ate:", {"500 milhas nauticas da costa", "12 milhas nauticas da costa", "50 milhas nauticas da costa", "200 milhas nauticas da costa"}, 3, HARD, 50};
    questions[22] = (Question){"O que e o 'Rendimento Maximo Sustentavel' (MSY) na gestao da pesca?", {"A maior quantidade de peixes que pode ser capturada sem esgotar o estoque", "O lucro maximo que uma empresa de pesca pode ter por lei", "O peso maximo que um unico barco de pesca pode transportar", "A velocidade maxima permitida para barcos de pesca em alto mar"}, 0, HARD, 50};
    questions[23] = (Question){"Qual organismo microscopico e a base da maior parte da cadeia alimentar marinha e produz oxigenio?", {"Krill", "Fitoplancton", "Zooplancton", "Bacterias marinhas"}, 1, HARD, 50};
    questions[24] = (Question){"A 'hipoxia', que cria as 'zonas mortas' no oceano, e uma condicao de:", {"Falta de luz solar no fundo do mar", "Agua com niveis extremamente baixos de oxigenio", "Excesso de sal na agua, tornando-a inabitavel", "Temperatura da agua abaixo de zero"}, 1, HARD, 50};
    questions[25] = (Question){"Qual a principal reacao quimica que descreve a acidificacao dos oceanos?", {"O2 + 2H2 -> 2H2O", "NaCl -> Na+ + Cl-", "CO2 + H2O -> H2CO3", "CaCO3 -> CaO + CO2"}, 2, HARD, 50};
    questions[26] = (Question){"A decada de 2021-2030 foi proclamada pela ONU como a Decada da:", {"Exploracao de Recursos Minerais Marinhos", "Protecao das Aves Marinhas", "Navegacao Sustentavel", "Ciencia Oceanica para o Desenvolvimento Sustentavel"}, 3, HARD, 50};
    questions[27] = (Question){"A 'biomagnificacao' e um processo perigoso onde:", {"Toxinas se acumulam em concentracoes maiores ao longo da cadeia alimentar", "Organismos marinhos crescem a um tamanho anormal", "A biodiversidade de uma area aumenta rapidamente", "A quantidade de sal aumenta em um organismo"}, 0, HARD, 50};
    questions[28] = (Question){"Qual destes subsidios a pesca a ODS 14.6 busca eliminar?", {"Subsidios para combustivel de pequenos pescadores", "Subsidios que contribuem para a sobrepesca e a pesca ilegal", "Financiamento para pesquisas sobre a vida marinha", "Ajuda de custo para a seguranca dos pescadores"}, 1, HARD, 50};
    questions[29] = (Question){"A 'termoclina' e uma camada no oceano onde ocorre uma rapida mudanca de:", {"Salinidade", "Pressao", "Temperatura", "Visibilidade"}, 2, HARD, 50};
    
    // Novas Perguntas
    questions[30] = (Question){"Qual o maior animal que ja existiu na Terra?", {"Dinossauro T-Rex", "Elefante Africano", "Baleia Azul", "Tubarão Megalodon"}, 2, EASY, 10};
    questions[31] = (Question){"Como os peixes respiram debaixo d'agua?", {"Segurando o ar por muito tempo", "Atraves de guelras (branquias)", "Pela pele", "Eles nao precisam respirar"}, 1, EASY, 10};
    questions[32] = (Question){"O que é um grande grupo de peixes nadando juntos?", {"Rebanho", "Cardume", "Alcateia", "Bando"}, 1, EASY, 10};
    questions[33] = (Question){"O que causa as marés (a subida e descida do nível do mar)?", {"O vento forte", "A rotação da Terra", "Correntes marítimas", "A gravidade da Lua e do Sol"}, 3, EASY, 10};
    questions[34] = (Question){"O que é a bioluminescência em animais marinhos?", {"Um tipo de camuflagem", "A capacidade de produzir luz própria", "A habilidade de respirar fora d'água", "Um som usado para comunicação"}, 1, MEDIUM, 25};
    questions[35] = (Question){"A Grande Barreira de Corais, o maior sistema de recifes do mundo, fica na costa de qual país?", {"Brasil", "Africa do Sul", "Japão", "Australia"}, 3, MEDIUM, 25};
    questions[36] = (Question){"Qual é o nome do processo em que a água do oceano evapora, forma nuvens e depois volta como chuva?", {"Fotossíntese", "Ciclo da água", "Efeito estufa", "Erosão"}, 1, MEDIUM, 25};
    questions[37] = (Question){"Qual é o ponto mais profundo conhecido nos oceanos da Terra?", {"Fossa das Marianas", "Fossa de Tonga", "Fossa do Japão", "Abismo de Challenger"}, 0, HARD, 50};
    questions[38] = (Question){"Na pesca, o que significa o termo 'bycatch' (captura acidental)?", {"Pescar mais do que o permitido", "Usar redes de pesca ilegais", "Animais marinhos capturados sem querer", "Um tipo de peixe raro"}, 2, HARD, 50};
    questions[39] = (Question){"Qual convenção internacional é o principal acordo para a prevenção da poluição do ambiente marinho por navios?", {"Convenção de Estocolmo", "Protocolo de Kyoto", "Acordo de Paris", "Convenção MARPOL"}, 3, HARD, 50};

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