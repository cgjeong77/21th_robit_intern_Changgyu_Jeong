import torch
import matplotlib.pyplot as plt
from torch import nn
from torch.utils.data import DataLoader, random_split
from torchvision import datasets, transforms

# 실험 재현을 위한 랜덤 시드 설정
torch.manual_seed(0)

# MNIST 학습 데이터 불러오기 및 텐서 변환
full = datasets.MNIST("data", train=True, download=True,
                transform=transforms.ToTensor())

# 학습 데이터 55,000개와 검증 데이터 5,000개로 분리
train, val = random_split(full, [55000, 5000],
    generator=torch.Generator().manual_seed(0))

# MNIST 테스트 데이터 불러오기
test = datasets.MNIST("data", train=False, download=True,
                transform=transforms.ToTensor())

# 학습 데이터를 64개씩 묶고 순서를 무작위로 섞음
train_loader = DataLoader(train, batch_size=64, shuffle=True)

# 검증 데이터를 256개씩 묶음
val_loader = DataLoader(val, batch_size=256)

# 테스트 데이터를 256개씩 묶음
test_loader = DataLoader(test, batch_size=256)


# CPU 또는 GPU 학습 장치 설정
device = torch.device(
    "cuda" if torch.cuda.is_available() else "cpu")

# MLP 신경망 모델 구성 (784 → 128 → 10)
model = nn.Sequential(
    nn.Flatten(),         # 28×28 이미지를 784개 값으로 변환
    nn.Linear(784, 128), # 입력값 784개를 은닉층 128개로 변환
    nn.ReLU(),           # 활성화 함수 적용
    nn.Linear(128, 10)   # 숫자 0~9에 대한 점수 출력
).to(device)

# 손실 함수 설정
criterion = nn.CrossEntropyLoss()

# Adam Optimizer 및 학습률 설정
optimizer = torch.optim.Adam(model.parameters(), lr=0.001)

# 학습 반복 횟수 설정
epochs = 10


# 검증 및 테스트 데이터 평가 함수
def evaluate(loader):
    model.eval()  # 모델을 평가 모드로 전환
    loss_sum, correct, count = 0.0, 0, 0

    # 평가 중에는 기울기를 계산하지 않음
    with torch.no_grad():
        for images, labels in loader:
            images, labels = images.to(device), labels.to(device)

            logits = model(images)  # 예측 점수 계산

            # 전체 손실 합산
            loss_sum += criterion(logits, labels).item() * len(labels)

            # 예측값과 정답을 비교하여 맞힌 개수 계산
            correct += (logits.argmax(1) == labels).sum().item()
            count += len(labels)

    # 평균 Loss와 Accuracy 반환
    return loss_sum / count, correct / count


# Epoch별 학습 및 검증 Loss 저장
train_losses = []
val_losses = []

# 설정한 Epoch 수만큼 전체 학습 반복
for epoch in range(epochs):
    model.train()  # 모델을 학습 모드로 설정
    total_loss = 0.0  # Epoch별 손실 합계 초기화

    # 학습 데이터를 미니배치 단위로 불러오기
    for images, labels in train_loader:
        images = images.to(device)  # 이미지 데이터를 학습 장치로 이동
        labels = labels.to(device)  # 정답 데이터를 학습 장치로 이동

        optimizer.zero_grad()  # 이전 기울기 초기화
        logits = model(images)  # Forward: 예측 점수 계산
        loss = criterion(logits, labels)  # Loss 계산
        loss.backward()  # Backpropagation: 기울기 계산
        optimizer.step()  # Optimizer: 가중치와 편향 갱신

        # 미니배치별 손실을 전체 손실에 누적
        total_loss += loss.item() * images.size(0)

    # 전체 학습 데이터에 대한 평균 Loss 계산
    train_loss = total_loss / len(train)

    
    # 검증 데이터의 Loss와 Accuracy 계산
    val_loss, val_acc = evaluate(val_loader)

    # Epoch별 Loss 기록
    train_losses.append(train_loss)
    val_losses.append(val_loss)

    # Epoch별 학습 및 검증 결과 출력
    print(f"Epoch {epoch + 1}: Train Loss={train_loss:.4f}, "
          f"Val Loss={val_loss:.4f}, Val Accuracy={val_acc:.4f}")

    
# 학습 완료 후 테스트 데이터로 최종 성능 평가
test_loss, test_acc = evaluate(test_loader)

# 최종 테스트 결과 출력
print(f"Test Loss: {test_loss:.4f}")
print(f"Test Accuracy: {test_acc * 100:.2f}%")


# Epoch별 학습 및 검증 Loss 그래프 생성
plt.plot(range(1, epochs + 1), train_losses, label="Train Loss")
plt.plot(range(1, epochs + 1), val_losses, label="Validation Loss")

# 그래프 제목 및 축 설정
plt.title("MNIST Training and Validation Loss")
plt.xlabel("Epoch")
plt.ylabel("Loss")
plt.legend()
plt.grid(True)

# 그래프 이미지 저장
plt.savefig("loss_graph.png", dpi=300, bbox_inches="tight")

# 오분류 이미지 수집
model.eval()
wrong_images = []

with torch.no_grad():
    for images, labels in test_loader:
        images = images.to(device)
        labels = labels.to(device)

        # 모델의 예측값 계산
        logits = model(images)
        predictions = logits.argmax(1)

        # 예측이 틀린 이미지 추출
        for i in range(len(labels)):
            if predictions[i] != labels[i]:
                wrong_images.append((
                    images[i].cpu(),
                    labels[i].item(),
                    predictions[i].item()
                ))

            # 오분류 이미지 9개 수집
            if len(wrong_images) >= 9:
                break

        if len(wrong_images) >= 9:
            break

# 오분류 이미지 시각화
fig, axes = plt.subplots(3, 3, figsize=(9, 9))

for ax, (image, label, pred) in zip(axes.flat, wrong_images):
    ax.imshow(image.squeeze(), cmap="gray")
    ax.set_title(f"True: {label}, Pred: {pred}")
    ax.axis("off")

plt.tight_layout()

# 오분류 이미지 저장
plt.savefig("misclassified_examples.png", dpi=300)
plt.show()