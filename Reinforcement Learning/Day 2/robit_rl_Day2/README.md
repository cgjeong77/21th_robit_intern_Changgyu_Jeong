
# FrozenLake Q-Learning 강화학습

## 1. 프로젝트 개요

ROBIT 강화학습 2일차 과제로, Python과 Gymnasium을 이용하여 FrozenLake 환경에서 Q-Learning 알고리즘을 구현했습니다.

FrozenLake는 4×4 격자에서 구멍을 피해 시작 지점(S)부터 목표 지점(G)까지 이동하는 환경입니다. Q-table과 ε-greedy 방식을 이용해 에이전트가 목표까지 이동하는 방법을 학습하도록 했습니다.

## 2. 실행 환경

- OS: Ubuntu 24.04
- Language: Python 3
- Library: Gymnasium, NumPy, Matplotlib

필요한 라이브러리 설치:

```bash
python -m pip install gymnasium numpy matplotlib
```

## 3. 학습 설정

| 항목 | 설정값 |
| --- | --- |
| 환경 | FrozenLake-v1 |
| 맵 크기 | 4×4 |
| 미끄러짐 | False |
| 상태 수 | 16 |
| 행동 수 | 4 |
| 알고리즘 | Q-Learning |
| 학습률(α) | 0.1 |
| 할인율(γ) | 0.95 |
| 학습 횟수 | 5,000 Episode |
| 탐험 확률(ε) | max(0.05, 0.999^episode) |
| 평가 횟수 | 200 Episode |
| 평가 탐험 확률 | 0.0 |

## 4. 주요 구현 내용

### 4.1 FrozenLake 환경 및 Q-table

`is_slippery=False`로 설정하여 에이전트가 선택한 방향대로 이동하도록 했습니다.

16개의 상태와 4개의 행동에 대한 Q값을 저장하기 위해 16×4 크기의 Q-table을 생성하고, 모든 값을 0으로 초기화했습니다.

### 4.2 ε-greedy 행동 선택

ε-greedy 방식을 사용하여 탐험과 활용을 함께 진행했습니다.

- 탐험: ε 확률로 무작위 행동 선택
- 활용: 1-ε 확률로 Q값이 가장 높은 행동 선택

학습이 진행될수록 ε값이 감소하도록 설정하여 점차 학습된 Q값을 활용하도록 했습니다.

### 4.3 Q-Learning 학습

총 5,000번의 Episode 동안 행동에 따른 보상과 다음 상태의 Q값을 이용하여 기존 Q값을 갱신했습니다.

Q값 갱신식:

`Q(s,a) ← Q(s,a) + α × (target - Q(s,a))`

일반적인 상태에서:

`target = r + γ × max Q(s',a')`

종료 상태에서는 다음 상태의 Q값을 더하지 않고 실제 보상만 사용했습니다.

학습된 Q-table은 `q_table.npy` 파일로 저장했습니다.

### 4.4 학습 결과 평가

저장된 Q-table을 불러온 뒤, 탐험 확률을 0으로 설정하고 총 200번의 Episode를 평가했습니다.

각 Episode의 성공 여부와 이동 횟수를 기록하여 성공률, 평균 경로 길이, 최소·최대 경로 길이를 계산했습니다.

## 5. 실행 방법

프로젝트 폴더에서 다음 명령어를 실행합니다.

```bash
python frozenlake_train.py
```

프로그램을 실행하면 Q-Learning 학습과 평가가 진행되고, 최종 평가 결과가 터미널에 출력됩니다.

## 6. 실행 결과

```text
Evaluation Success Rate: 100.00%
Successful Episodes: 200/200
Average Path Length: 6.00
Min Path Length: 6
Max Path Length: 6
```

200번의 평가에서 모두 목표 지점에 도달하여 성공률 100%를 기록했습니다.

평균·최소·최대 경로 길이는 모두 6회로 나타났으며, 시작 지점에서 목표 지점까지 최단 경로로 이동하는 것을 확인했습니다.

학습 과정에서는 초기 성공률이 낮았지만, Episode가 증가하면서 성공률이 점차 높아졌습니다. 학습 후반에는 500 Episode 단위 평균 성공률이 약 93~94% 수준으로 유지되었습니다.

다만 미끄러짐이 없는 환경에서 탐험 없이 평가한 결과이므로, 다른 환경에서도 동일한 성능이 나타난다고 볼 수는 없습니다.

## 7. 프로젝트 파일

```text
robit_rl_day2/
├── frozenlake_train.py
├── q_table.npy
└── README.md
```

- `frozenlake_train.py`: Q-Learning 학습, 평가 및 시각화 코드
- `q_table.npy`: 학습된 Q-table 데이터
- `README.md`: 프로젝트 설명 및 실행 방법

실행 시 학습 결과를 나타내는 그래프와 정책 격자 이미지도 생성되지만, README에는 별도로 삽입하지 않았습니다.

## 8. AI 활용 및 참고 자료

ROBIT 강화학습 2일차 강의자료를 중심으로 과제를 진행했습니다.

ChatGPT는 MDP, Return, TD target, ε-greedy 등의 개념을 이해하고, Q값 갱신 수식을 확인하는 데 활용했습니다. 또한 학습 결과 시각화 코드 작성과 실행 결과 분석, 보고서 정리 과정에서도 도움을 받았습니다.

### 참고 자료

1. ROBIT, 「MDP와 Q-Learning 강화학습 2일차」, 2026.10.29.
2. Gymnasium Documentation, Solving Frozenlake with Tabular Q-Learning  
   https://gymnasium.farama.org/tutorials/training_agents/frozenlake_q_learning/
3. OpenAI, ChatGPT
