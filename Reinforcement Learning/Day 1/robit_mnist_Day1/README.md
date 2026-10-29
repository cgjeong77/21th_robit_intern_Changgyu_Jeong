# MNIST 손글씨 숫자 분류

## 1. 프로젝트 소개

PyTorch를 이용하여 MNIST 손글씨 숫자(0~9)를 분류하는 MLP 신경망을 구현했다. 모델 학습 후 Epoch와 Learning Rate를 변경하여 성능을 비교하고, Loss 그래프와 오분류 이미지를 확인했다.

## 2. 개발 환경

- OS: Ubuntu 24.04
- Language: Python 3.12
- Framework: PyTorch
- Libraries: torchvision, matplotlib

## 3. 모델 구조 및 학습 설정

- 데이터셋: MNIST (28×28 흑백 이미지)
- 학습 데이터: 55,000개
- 검증 데이터: 5,000개
- 테스트 데이터: 10,000개
- 모델 구조: 784 → 128 → 10
- 활성화 함수: ReLU
- 손실 함수: CrossEntropyLoss
- Optimizer: Adam
- Batch Size: 학습 64, 검증 및 테스트 256

### 모델 구조 선택 이유

MNIST 이미지가 28×28 크기이므로 입력값을 784개로 설정하고, 숫자 0부터 9까지 분류하기 위해 출력값을 10개로 설정했다. 은닉층은 교육 자료를 참고하여 128개의 뉴런으로 구성했으며, ReLU 활성화 함수를 사용했다.

### 학습 설정 선택 이유

손실 함수는 10개의 숫자를 분류하는 문제이므로 CrossEntropyLoss를 사용했다. Optimizer는 교육 자료에서 사용한 Adam을 적용했다. Adam은 기울기를 이용하여 각 파라미터의 갱신 크기를 적응적으로 조절하는 알고리즘이다.

학습률은 교육 자료의 기본 설정인 0.001을 사용했다. 이후 학습률을 0.0005로 낮추거나 Epoch를 늘려 성능이 어떻게 달라지는지 비교했다.

## 4. 실행 방법

필요한 라이브러리 설치:

```bash
pip install torch torchvision matplotlib
```

학습 코드 실행:

```bash
python mnist_train.py
```

MNIST 데이터셋은 실행 시 자동으로 다운로드된다. 학습 조건은 코드에서 `lr`과 `epochs` 값을 수정하여 변경할 수 있다.

## 5. 실험 결과

| 실험 | Epoch | Learning Rate | Test Accuracy |
|---|---:|---:|---:|
| 실험 1 | 5 | 0.001 | 97.46% |
| 실험 2 | 10 | 0.001 | 97.55% |
| 실험 3 | 5 | 0.0005 | 96.81% |

실험 2에서 가장 높은 테스트 정확도인 97.55%를 기록했다. 다만 학습 후반에는 Validation Loss가 다시 증가하는 모습이 나타났다.

## 6. 결과 파일

- `mnist_train.py`: 모델 학습 및 평가 코드
- `results/`: 실험별 학습 로그와 결과 이미지
- `loss_graph.png`: Train Loss 및 Validation Loss 그래프
- `misclassified_examples.png`: 오분류 숫자 9개

## 7. AI 활용 및 참고 자료

MNIST 데이터셋 준비, MLP 모델 구성, 학습 및 평가 코드는 교육 자료를 참고하여 작성하고 ChatGPT로 검토했다.

Loss 그래프 생성 코드와 오분류 숫자 9개를 추출하고 출력하는 코드는 ChatGPT의 도움을 받아 추가했다. 보고서의 구성과 설명을 정리할 때도 ChatGPT를 활용했다.

참고 자료:
- ROBIT 강화학습 교육 1일차 강의 자료
- https://noru-jumping-in-the-mountains.tistory.com/10
