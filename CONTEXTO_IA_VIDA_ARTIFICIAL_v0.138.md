# Vida Artificial Evolutiva — Contexto de Manutenção para IA

**Versão autoritativa:** Aprendizado com Reforço v0.138  
**Arquivo:** `aprendizado_com_reforco_v0.138.cpp`  
**Objetivo:** preservar regras, migrações e intenção experimental. Sempre ler este arquivo e o contrato no topo do `.cpp` antes de alterar o projeto.

## 1. Ordem de autoridade

1. Pedido explícito mais recente do usuário.
2. Contrato de manutenção no topo do `.cpp`.
3. Este documento.
4. Código atual.
5. Comentários/changelogs históricos.

Não tentar manter silenciosamente duas regras conflitantes.

---

## 2. Filosofia

O projeto não deve hardcodar inteligência complexa. O ambiente fornece sensores, ações, custos e reprodução; mutação, seleção, plasticidade e aprendizado devem produzir o comportamento.

A v0.138 muda o princípio do cérebro: ele deixa de ser pensado como uma MLP obrigatoriamente preenchida e passa a ser um **grafo neural esparso evolutivo**.

Complexidade deve ser possível, mas não gratuita nem inevitável.

---

# 3. REFORMA NEURAL v0.138 — REGRA CENTRAL

## 3.1 Três níveis visuais/sensoriais

A visão possui três níveis diferentes. Não confundir:

```text
MUNDO
  ↓
RAYCASTS / RETINA BRUTA
  ↓
PERCEPÇÃO CONTÍNUA
  ↓
CÉREBRO ESPARSO
```

### Retina bruta

Os raycasts continuam sendo a verdade de visibilidade. Um objeto só é percebido se algum raio realmente o atingir.

A Retina 1D do popup continua mostrando as faixas dos raios reais.

### Percepção contínua

Depois de um objeto ser detectado por um raio, o sistema pode usar a identidade/posição do objeto detectado para fornecer sinais contínuos ao cérebro.

Atualmente existem canais contínuos para:

- planta: presença, direção relativa, proximidade;
- bicho: presença, direção relativa, proximidade;
- carne: presença, direção relativa, proximidade;
- ovo: presença, direção relativa, proximidade;
- contato da boca com planta: sensor tátil local.

Direção:

- `-1` = esquerda;
- `0` = frente;
- `+1` = direita.

Proximidade:

- `0` = distante;
- `1` = próximo.

**Regra anti-trapaça:** não consultar o mapa para informar onde está um objeto invisível. Primeiro um raycast precisa detectá-lo. Se nenhum raio detecta o alvo naquele tick, a percepção visual dele desaparece. Persistência depois da perda visual deve vir de memória/cérebro, não do sensor.

A posição do centro do objeto detectado é retornada também pelo caminho GPU para calcular direção relativa contínua sem depender apenas do índice do raio.

## 3.2 Catálogo fixo, expressão genética variável

Por performance e compatibilidade, o programa mantém um catálogo máximo fixo de inputs/outputs.

Mas o DNA possui:

- `activeInputs`;
- `activeOutputs`.

Portanto um indivíduo **não nasce possuindo todos os sensores e ações**.

Um input/output pode:

- estar dormente;
- surgir por mutação;
- desaparecer por mutação;
- existir ativo sem ligação alguma;
- ser recrutado por uma conexão em geração posterior.

Isso é intencional. Gene isolado não é bug.

Inputs de percepção contínua têm maior probabilidade de surgir que detalhes extremamente específicos da retina bruta. Canais brutos por raio continuam evolutivamente disponíveis, apenas mais difíceis.

## 3.3 Neurônios ocultos

Fundadores da v0.138 nascem com **zero neurônios ocultos**.

Descendentes podem ganhar neurônios por mutação.

Um neurônio novo pode nascer isolado. O padrão atual torna isso frequente de propósito. Ele pode permanecer inútil por várias gerações até uma ligação futura recrutá-lo.

Neurônios continuam tendo:

- área;
- camada/profundidade usada para organização e direção provável;
- bias;
- gain;
- ativação;
- memória temporal opcional.

Camada não é uma barreira rígida de MLP. É metadado que influencia recorrência/direção e visualização.

## 3.4 Áreas neurais

Áreas continuam existentes:

- Geral;
- Visual;
- Interna;
- Memória;
- Emoção;
- Motora;
- Social;
- Reservada/legado.

Uma ligação entre elementos da mesma área especializada é mais provável.

Cruzar áreas é permitido, apenas mais raro.

Isso cria modularidade evolutiva sem hardcodar módulos comportamentais.

## 3.5 Ligações são genes funcionais

Uma ligação possui origem, destino, peso e também uma **função de transferência**.

Funções atuais:

1. Linear;
2. Inversa (`-x`);
3. Absoluta;
4. Quadrática com sinal;
5. Senoidal;
6. Limiar;
7. Gaussiana;
8. Saturação (`tanh`).

Além disso continuam existindo os modos estruturais anteriores:

- Soma;
- Gate;
- Modulação;
- Shunt.

E a ligação pode ser plástica.

Portanto “função” e “modo” não são a mesma coisa:

```text
sinal da origem
   ↓
função da ligação
   ↓
peso aprendido/genético
   ↓
modo (soma/gate/mod/shunt)
   ↓
destino
```

## 3.6 Probabilidade de novas ligações

Regra desejada:

- para frente: mais fácil;
- mesma camada: menos fácil;
- para trás/recorrente: difícil;
- feedback de output: muito difícil;
- mesma área: favorecido;
- áreas diferentes: penalizado;
- quanto mais saídas um nó já possui, mais difícil ganhar outra conexão.

O fan-out portanto sofre penalidade progressiva. Isso ajuda a manter cérebros esparsos.

## 3.7 Complexidade custa energia

O metabolismo neural cobra por:

- neurônio oculto;
- conexão ativa;
- conexão recorrente (custo extra);
- neurônio oculto com memória temporal (custo extra);
- registrador persistente;
- conexão plástica (custo extra).

Objetivo: permitir que cérebro pequeno e eficiente vença um cérebro gigante que não oferece vantagem útil.

---

# 4. CÉREBRO ANCESTRAL

Todo fundador/criatura artificial inicial da arquitetura v0.138 recebe o mesmo núcleo mínimo.

Não nasce com todos os inputs, todos outputs ou hidden layers.

Inputs ativos iniciais:

- `Planta presente`;
- `Planta direção`;
- `Planta proximidade`;
- `Planta contato boca`.

Outputs ativos iniciais:

- Frente;
- Ré;
- Girar esquerda;
- Girar direita;
- Morder.

Conexões ancestrais:

```text
Planta presente ─────────────→ Frente

Planta direção ──────────────→ Girar direita
Planta direção ── peso oposto→ Girar esquerda

Planta proximidade ──────────→ Ré

Planta contato boca ─────────→ Morder
```

Efeito esperado:

1. enxerga planta;
2. direção relativa contínua faz virar para ela;
3. presença faz avançar;
4. proximidade ativa ré antagonista e reduz aproximação perto do alvo;
5. quando a boca realmente toca planta, output Morder dispara.

A mordida não é mais uma ação automática para cérebros v0.138: virou output neural evolutivo.

O ancestral possui:

- 0 hidden;
- 0 registradores persistentes;
- apenas os genes sensoriais/motores acima ativos.

Descendentes podem alterar, adicionar ou perder qualquer parte segundo as mutações estruturais permitidas.

---

# 5. O QUE PODE EVOLUIR A PARTIR DO ANCESTRAL

Por nascimento/mutação podem surgir independentemente:

- input novo sem ligação;
- output novo sem ligação;
- neurônio oculto isolado;
- conexão nova;
- remoção/desativação de conexão;
- reativação de conexão dormente;
- mudança de peso;
- mudança da função da ligação;
- mudança dos parâmetros da função;
- mudança de modo Soma/Gate/Mod/Shunt;
- recorrência;
- nova área de neurônio;
- nova ativação de neurônio;
- memória temporal;
- registradores persistentes;
- plasticidade;
- duplicação de neurônio/circuito;
- divisão de conexão por um novo neurônio;
- novos outputs como agarrar, acasalar, cor e memória;
- novos inputs internos, emocionais, episódicos, perceptivos e de retina bruta.

Nem toda mutação precisa ser útil no nascimento em que surgiu.

---

# 6. EMOÇÕES

Emoções **não foram redesenhadas na v0.138**.

Continuam como estavam e permanecem dentro de `Cérebro > Emoções`.

Inputs/outputs dormentes não devem fornecer sinal secreto para triggers emocionais. Se o gene não está expresso, o trigger correspondente lê zero.

Não criar um modal de emoções separado.

---

# 7. MEMÓRIA, PLASTICIDADE E HERANÇA

Continuam existindo:

- estado recorrente/temporal dos hidden;
- 4 registradores persistentes possíveis, mas capacidade é genética;
- memória episódica;
- plasticidade reward-modulated;
- pesos aprendidos durante a vida.

Filhos de acasalamento herdam conhecimento dos dois pais:

- aprendizado sináptico estruturalmente correspondente;
- registradores persistentes;
- memória episódica consolidada.

Estados momentâneos (energia, dano recente, reward pendente, eligibility recente) não são conhecimento herdável.

Ovo carrega o conhecimento herdável para ele sobreviver à morte dos pais e a save/load durante incubação.

---

# 8. SAVE / MIGRAÇÃO v0.138

`PROGRAM_VERSION = 135`.

`NeuralConnectionGene` aumentou porque passou a guardar:

- `function`;
- `paramA`;
- `paramB`.

Para saves `<135`, usar `LegacyNeuralConnectionGeneV134` e converter explicitamente.

Não ler vetor antigo diretamente como o struct novo.

Saves antigos:

- mantêm seus inputs/outputs históricos expressos;
- novos canais contínuos começam dormentes, exceto contato de boca com planta usado para preservar mordida;
- o antigo slot output 7 era inerte. Conexões históricas para/dele são desativadas antes de promover o slot a `Morder`, evitando transformar lixo histórico em comportamento;
- uma ligação forte `Planta contato boca -> Morder` é inserida na migração.

`activeInputs` e `activeOutputs` são serializados apenas no layout v0.138+.

Nunca mudar `NeuralConnectionGene`, `NeuralGenome`, índices de input/output ou structs POD de save sem nova migração.

---

# 9. VISÃO GPU

A visão principal continua D3D11 Compute Shader e respeita o mundo toroidal.

A v0.138 aumentou `GpuVisionResult` para também retornar o centro mundial do objeto detectado. CPU e GPU devem manter a mesma semântica.

**Limitação de validação local:** o `g++` Linux usado pelo assistente não compila/executa os blocos `_WIN32` nem valida HLSL em runtime. Testar no Windows se o profiler mostra GPU COMPUTE e se a percepção contínua muda suavemente ao girar para um alvo.

---

# 10. MUNDO

O mundo é **toroidal**:

- direita conecta à esquerda;
- cima conecta embaixo;
- não existe parede sensorial;
- raycasts atravessam a costura;
- proximidade, física, mordida, visão e acasalamento devem usar distância/topologia toroidal.

Para o bicho, o mundo deve parecer contínuo.

---

# 11. SOM

Não existe mecânica ativa de som.

Não reintroduzir:

- eventos sonoros;
- audição;
- campo acústico;
- vocalização;
- custos acústicos;
- inputs de áudio;
- genes acústicos ativos.

Slots históricos permanecem apenas quando necessários para compatibilidade binária e ficam inertes/zerados.

---

# 12. REPRODUÇÃO — RESUMO ATUAL

Duas rotas:

## Solo

- custo integral;
- 90% nasce;
- 10% vira carne;
- funciona como fallback.

## Acasalamento

- prioridade sobre solo;
- cabeça com cabeça;
- intenção/drive de acasalamento;
- 70% menos custo principal;
- 0% falha biológica;
- incubação pela metade;
- qualidade >=90% boa, 100% excelente;
- pode gerar ninhada de 1 até 10;
- herda conhecimento dos dois pais;
- linhagem sexual espera muito mais antes de recorrer a ovo solo.

Ovo possui `50%` do antigo tamanho físico/visual.

O gráfico `Ovos atuais` mostra apenas quantos ovos SOLO e ACASALADOS existem naquele instante, não total acumulado.

---

# 13. PLANTAS — RESUMO

- raiz em `Plant::pos` é fixa;
- sway move apenas copa visual;
- planta mordida não volta a crescer;
- tamanho muda na mordida, não por encolhimento contínuo;
- se ainda tiver biomassa suficiente, planta parcialmente comida pode continuar reproduzindo mais lentamente;
- plantas não voltam ao solver físico planta-planta.

---

# 14. UI / OBSERVAÇÃO

Ao seguir um bicho:

- `Cérebro` abre laboratório;
- `Memórias` abre visualização observacional;
- `Visão` abre Retina 1D;
- Emoções ficam como aba do Cérebro.

Na Retina 1D:

- cada raio é uma faixa;
- a cor enfraquece com distância;
- legenda identifica o tipo;
- a v0.138 também mostra a percepção contínua da planta para comparar retina bruta com o vetor simplificado entregue ao cérebro.

No laboratório neural:

- genes de input/output ativos devem ser visualmente mais fortes;
- genes dormentes devem aparecer apagados;
- hidden mostra área;
- conexões mostram recorrência/plasticidade/modo;
- editor permite observar/ciclar função e parâmetros da ligação.

---

# 15. PERFORMANCE

- visão GPU é caminho principal;
- não reintroduzir loops O(N²) globais para sensores/interações;
- manter grids espaciais;
- cérebro é esparso: executar somente conexões compiladas ativas;
- input/output dormente não entra na rede compilada;
- não voltar a uma matriz totalmente conectada por padrão.

---

# 16. TESTE MÍNIMO ANTES DE PUBLICAR

Executar:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wreturn-type -Werror -fsyntax-only aprendizado_com_reforco_v0.138.cpp
```

Além disso, no Windows:

1. confirmar GPU COMPUTE no profiler;
2. criar novo mundo e verificar fundador com zero hidden;
3. verificar que planta à esquerda gera direção negativa e giro à esquerda;
4. planta à direita gera direção positiva e giro à direita;
5. proximidade reduz avanço;
6. contato real da boca ativa mordida;
7. observar descendentes ganhando inputs/outputs/hidden/conexões gradualmente;
8. salvar/carregar v0.138;
9. carregar ao menos um save v0.134 e conferir migração neural;
10. verificar que mundo toroidal continua correto na visão GPU.

---

# 17. NÃO “CORRIGIR” ESTAS COISAS

Os itens abaixo são intencionais:

- neurônio isolado;
- input ativo sem conexão;
- output ativo sem conexão;
- conexão dormente;
- cérebro com pouquíssimas conexões;
- recorrência rara;
- conexão entre áreas rara, mas possível;
- mutação inútil que só ganha função gerações depois;
- fundador sem hidden;
- percepção contínua desaparecer quando nenhum raycast vê o objeto.

Essas características são parte do espaço evolutivo, não bugs.


---

# 18. v0.136 — OBSERVABILIDADE DO CÉREBRO

O laboratório neural ganhou leitura ao vivo do fluxo:

- hover em input, hidden, output e ligação mostra nome completo e valor atual;
- ligações mostram origem, função, peso aprendido e contribuição instantânea;
- hidden mostra estado, bias, ganho, memória, camada e área;
- inputs/outputs têm barras de intensidade;
- o topo lista entradas e saídas mais fortes do instante;
- a espessura/opacidade das conexões leva em conta influência atual, não somente peso.

Essa UI é apenas observacional/editorial e não muda a biologia.

---

# 19. v0.138 — DATA-ORIENTED / IDEIAS DO “MAGIC CONTAINER”

A referência estudada foi o StableIndexVector/ConstantIndexVector do johnBuffer, associado ao vídeo “The magic container”. A ideia central é manter identidade estável sem abrir mão de iteração amigável a cache: dados/índices densos, remoção por swap e lookup direto por ID.

A v0.138 aplica esses princípios de forma compatível com a arquitetura existente, sem invalidar os muitos ponteiros temporários usados pelo simulador:

- `SpatialGrid` deixou de usar `unordered_map<GridKey, vector<T*>>` no caminho quente e virou uma tabela densa de células, com uma lista compacta de células realmente ativas;
- ativar/desativar célula é O(1) por `activeCellSlot`, usando swap-pop;
- `forCircle()` toroidal não cria mais `unordered_set<T*>` por consulta: cada célula embrulhada é visitada no máximo uma vez;
- broadphase da física também deixou o hash map e passou a buckets densos reutilizáveis + lista de células ativas;
- `byId` de criaturas deixou de ser `unordered_map<int, Creature*>` e passou a lookup vetorial direto por ID;
- `StableObjectPool` continua preservando endereços (necessário porque o código ainda possui referências por ponteiro), mas cada slot agora tem `localityKey`;
- a cada ~10 s as listas de ponteiros ativos são reordenadas por `localityKey`, fazendo os loops percorrerem os blocos do pool em ordem física muito mais próxima da memória, sem mover os objetos nem invalidar grids/referências.

## Por que NÃO trocar tudo imediatamente por `std::vector<T>` + swap-pop

O projeto ainda contém referências persistentes como `grabbedCreature`, `grabbedCarcass`, `grabbedPlant` e buckets espaciais com ponteiros. Mover os próprios objetos em um vector invalidaria esses endereços. A v0.138 adota primeiro as vantagens seguras do modelo do vídeo. Uma migração futura completa para handles/IDs pode então tornar os próprios objetos 100% densos.

## Invariantes de performance novos

- não reintroduzir hash por célula no `SpatialGrid` sem benchmark que justifique;
- não alocar `unordered_set` dentro de consultas espaciais por criatura;
- não trocar pools estáveis por listas/árvores com pointer chasing;
- ao migrar entidades para armazenamento totalmente denso no futuro, substituir referências persistentes por handles/IDs antes de permitir que objetos se movam na memória.

## Teste desta versão

Além dos testes anteriores, comparar no profiler principalmente:

- sensores/proximidade;
- acasalamento;
- mordidas;
- física buckets/pairs;
- movimento total;
- RAM de `Sensores/grades` e `Fisica buffers`.

O ganho exato depende da população e do tamanho do mapa; não assumir uma porcentagem sem medir no Windows.


## v0.138 — visualizacao da grade de celulas
- O tile fundamental do mundo toroidal agora desenha no fundo uma **grade quadrada** baseada em `cfg::CELL`.
- A grade e adaptativa ao zoom: quando a camera esta muito afastada, linhas menores sao agrupadas para nao virar ruido visual.
- Linhas principais aparecem a cada 4 celulas para facilitar leitura espacial do mundo, dos grids e do wrapping toroidal.
- A costura do mundo continua marcada separadamente pelo retangulo externo.
