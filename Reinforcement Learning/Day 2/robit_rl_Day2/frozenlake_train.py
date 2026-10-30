import gymnasium as gym
import numpy as np
import matplotlib.pyplot as plt


# FrozenLake 4×4 환경 생성 (미끄러짐 없음)
env = gym.make("FrozenLake-v1",
               map_name="4x4", is_slippery=False)

# 랜덤 시드 설정
rng = np.random.default_rng(0)

# 상태 16개, 행동 4개의 Q-table 생성 및 초기화
Q = np.zeros((env.observation_space.n,
              env.action_space.n))

# 학습률과 할인율 설정
alpha, gamma = 0.1, 0.95

# Episode별 누적 보상 저장
returns = []


# ε-greedy 방식으로 행동 선택
def choose_action(state, epsilon, rng=rng):

    # 확률 epsilon으로 무작위 행동 선택 (탐험)
    if rng.random() < epsilon:
        return int(rng.integers(4))

    # 현재 상태에서 Q 값이 가장 큰 행동 탐색
    best = np.flatnonzero(
        Q[state] == Q[state].max())

    # Q 값이 가장 큰 행동 중 무작위 선택 (활용)
    return int(rng.choice(best))


# 총 5,000번의 Episode 동안 학습
for episode in range(5000):

    # 환경 초기화 및 현재 상태 가져오기
    state, _ = env.reset(seed=0 if episode == 0 else None)

    # Episode가 진행될수록 탐험 확률 감소
    epsilon = max(0.05, 0.999 ** episode)
    total = 0

    # Episode가 종료될 때까지 행동 반복
    while True:

        # ε-greedy 방식으로 행동 선택
        action = choose_action(state, epsilon)

        # 행동 실행 후 다음 상태, 보상, 종료 여부 확인
        ns, r, terminated, truncated, _ = env.step(action)

        # Q-Learning의 TD target 계산
        target = r if terminated else r + gamma * Q[ns].max()

        # Q 값 갱신
        Q[state, action] += alpha * (target - Q[state, action])

        # 보상 누적 및 현재 상태 갱신
        total += r
        state = ns

        # 목표 도달, 구멍 진입 또는 시간 제한 시 종료
        if terminated or truncated:
            break

    # Episode별 누적 보상 저장
    returns.append(total)


# 학습된 Q-table 파일 저장
np.save("q_table.npy", Q)


# 저장된 Q-table 불러오기
Q = np.load("q_table.npy")

# 평가용 FrozenLake 환경 생성
eval_env = gym.make("FrozenLake-v1",
                    map_name="4x4", is_slippery=False)

# 평가용 랜덤 시드 설정
eval_rng = np.random.default_rng(1000)

# 성공 횟수 및 성공 경로 길이 저장
wins = 0
path_lengths = []

# 학습된 Q-table로 총 200번 평가
for ep in range(200):

    # 평가 환경 초기화
    state, _ = eval_env.reset(seed=1000 if ep == 0 else None)
    steps = 0

    # 탐험 없이 목표 지점까지 이동
    while True:

        # Q 값이 가장 큰 행동 선택
        action = choose_action(state, 0.0, eval_rng)

        # 행동 실행 후 다음 상태와 보상 확인
        state, r, term, trunc, _ = eval_env.step(action)
        steps += 1

        # Episode 종료 시 성공 여부 확인
        if term or trunc:
            if r == 1:
                wins += 1
                path_lengths.append(steps)
            break


# 최종 평가 결과 출력
print(f"Evaluation Success Rate: {wins / 200 * 100:.2f}%")
print(f"Successful Episodes: {wins}/200")

# 성공한 Episode의 경로 길이 확인
if path_lengths:
    print(f"Average Path Length: {np.mean(path_lengths):.2f}")
    print(f"Min Path Length: {np.min(path_lengths)}")
    print(f"Max Path Length: {np.max(path_lengths)}")

# 학습 및 평가 환경 종료
eval_env.close()
env.close()

# 1. 학습 성공률 및 평균 보상 그래프

# 500 Episode마다 평균 성공률 및 평균 보상 계산
episodes = []
success_rates = []

for i in range(0, len(returns), 500):
    episodes.append(i + 500)
    success_rates.append(np.mean(returns[i:i + 500]))

# 학습 성공률 및 평균 보상 그래프 생성
plt.figure(figsize=(9, 5))
plt.plot(episodes, success_rates, marker="o")

# 그래프 제목 및 축 설정
plt.title("Training Success Rate and Average Reward")
plt.xlabel("Episode")
plt.ylabel("Success Rate / Average Reward")
plt.ylim(0, 1)
plt.grid(True)

# 그래프 이미지 저장
plt.savefig("training_success_rate.png", dpi=300,
            bbox_inches="tight")

# 2. Q-table 히트맵

# 학습된 Q-table 히트맵 생성
plt.figure(figsize=(8, 9))
plt.imshow(Q, cmap="YlGnBu", aspect="auto")

# 그래프 제목 및 축 설정
plt.title("FrozenLake Q-table Heatmap")
plt.xlabel("Action")
plt.ylabel("State")

# 행동 번호와 상태 번호 표시
plt.xticks(range(4), ["Left", "Down", "Right", "Up"])
plt.yticks(range(16))

# 각 상태와 행동의 Q값 표시
for s in range(16):
    for a in range(4):
        plt.text(a, s, f"{Q[s, a]:.2f}",
                 ha="center", va="center", fontsize=9)

# Q값에 따른 색상 표시
plt.colorbar(label="Q-value")
plt.tight_layout()

# 히트맵 이미지 저장
plt.savefig("q_table_heatmap.png", dpi=300)

# 3. 정책 격자 (Policy Grid)

# FrozenLake 4×4 맵 구성
lake_map = [
    "SFFF",
    "FHFH",
    "FFFH",
    "HFFG"
]

# 행동 번호에 따른 이동 방향
directions = ["←", "↓", "→", "↑"]

# 정책 격자 생성
fig, ax = plt.subplots(figsize=(6, 6))

for row in range(4):
    for col in range(4):

        # 행과 열을 상태 번호로 변환
        state = row * 4 + col
        tile = lake_map[row][col]

        # 구멍과 목표 지점은 별도로 표시
        if tile == "H" or tile == "G":
            symbol = tile

        # Q 값이 모두 0이면 학습되지 않은 상태로 표시
        elif np.allclose(Q[state], 0):
            symbol = "?"

        # Q 값이 가장 큰 행동을 방향으로 표시
        else:
            action = np.argmax(Q[state])
            symbol = directions[action]

        # 시작 위치 표시
        if tile == "S":
            symbol = "S\n" + symbol

        ax.text(col, row, symbol,
                ha="center", va="center", fontsize=20)

# 격자 눈금 및 제목 설정
ax.set_xticks(np.arange(-0.5, 4, 1), minor=True)
ax.set_yticks(np.arange(-0.5, 4, 1), minor=True)
ax.grid(which="minor", color="gray", linewidth=1)

ax.set_xlim(-0.5, 3.5)
ax.set_ylim(3.5, -0.5)
ax.set_xticks([])
ax.set_yticks([])
ax.set_title("FrozenLake Greedy Policy")

plt.tight_layout()

# 정책 격자 이미지 저장
plt.savefig("policy_grid.png", dpi=300,
            bbox_inches="tight")
plt.show()