
# Convenience Store Chain Database System

## Abstract

본 프로젝트는 **편의점 체인의 매장 및 온라인 구매를 관리하기 위한 데이터베이스 시스템**을 구현한 것이다.

MySQL 데이터베이스와 C++ 애플리케이션으로 구성되어 있으며, **재고 관리**, **판매 거래**, **고객 및 공급업체 데이터** 기능을 포함한다.

E-R 다이어그램을 기반으로 설계된 논리적 스키마는 **BCNF 정규화**를 적용하여 데이터 무결성과 쿼리 성능을 최적화했다.

애플리케이션은 7가지 샘플 쿼리를 지원하여 다양한 비즈니스 요구사항(예: 제품 재고 조회, 매출 분석, 고객 구매 패턴 분석)을 충족한다.

---

## 프로그램 설명

C++ 애플리케이션(`main.cpp`)은 **MySQL Connector**를 활용하여 데이터베이스와 연결되며, 다음과 같은 기능을 제공한다:

### 테이블 구성
- store : 매장 정보
- customer : 고객 정보
- vendor : 공급업체 정보
- product : 제품 정보
- sales_transtion : 거래 정보
- transaction_item : 거래별 구매 품목
- store_product : 매장별 재고 정보
- customer_email, customer_phone, product_category, vendor_contact, store_phone : Multivalued Attribute를 저장


### 주요 기능

- **데이터베이스 연결**  
  - MySQL 서버에 연결하여 데이터 읽기/쓰기 수행

- **Loyalty Status**
  - main함수 실행 이후에 소비한 금액이 $10 이상인 고객은 VIP로, $50 이상인 고객은 VVIP로 자동으로 업데이트 된다
  

- **쿼리 실행**  
  - 7가지 샘플 쿼리 실행

    1. 특정 제품의 재고를 보유한 매장 목록 조회  
    2. 2025년 5월 매장별 최고 판매 제품 식별  
    3. 2025년 4~6월 최고 매출 매장 조회  
    4. 제품 종류 기준 최대 공급 업체 식별  
    5. 재고 부족 제품 목록 조회  
    6. 특정 제품과 함께 구매된 상위 3개 제품 분석  
    7. 프랜차이즈 vs 직영 매장의 제품 다양성 비교

- **에러 처리**  
  - 연결 오류, 쿼리 실패, 사용자 입력 오류 등

- **결과 출력**  
  - 콘솔에 테이블 형식으로 출력

---

## 구현 환경

| 항목 | 내용 |
|------|------|
| 운영 체제 | Windows 10/11 (64-bit) |
| 언어/컴파일러 | C++17, MSVC (cl.exe) |
| 데이터베이스 | MySQL 8.0 |
| 라이브러리 | MySQL Connector C 6.1 (`libmysql.lib`) |
| 개발 도구 | Visual Studio Code (VSCode) |

### 설정 파일

- `settings.json`: 파일 연결 설정  
- `c_cpp_properties.json`: 헤더 경로 및 C++17 설정  
- `tasks.json`: MSVC 컴파일 작업 정의  
- `launch.json`: 디버깅 설정

---

## 컴파일 방법

### 1. 필수 구성 요소 설치

- **MySQL 8.0** 서버 설치 및 사용자/데이터베이스 설정
- **MySQL Connector C 6.1** 설치  
  - Include 경로: `C:/Program Files/MySQL/MySQL Connector C 6.1/include`  
  - Lib 경로: `C:/Program Files/MySQL/MySQL Connector C 6.1/lib/libmysql.lib`
- **Visual Studio 2022 Community Edition** 설치 (MSVC 포함)
- **Visual Studio Code** 및 C/C++ Extension 설치

### 2. 소스 코드 준비

- 프로젝트 폴더에 `main.cpp`, `schema.sql`, `sample_data.sql` 배치
- `.vscode` 폴더 내 설정 파일들을 프로젝트 루트에 복사

### 3. 데이터베이스 설정

- `schema.sql` 실행 → 테이블 생성
- `sample_data.sql` 실행 → 샘플 데이터 삽입
- `main.cpp` 내 DB 연결 정보(`server`, `user`, `password`, `database`) 수정
  - 현재에는 `localhost`, `root`, `1234`, `store`로 설정되어 있음


### 4. 컴파일 및 빌드

#### 자동 빌드

- VSC 터미널에서 `Ctrl + Shift + B` → tasks.json 기반 빌드  
  → `main.exe` 생성

#### 수동 컴파일

```bat
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cl /EHsc /Zi /W4 /Fe:main.exe main.cpp /I "C:\Program Files\MySQL\MySQL Connector C 6.1\include" /link /LIBPATH:"C:\Program Files\MySQL\MySQL Connector C 6.1\lib" libmysql.lib
```

---

## 실행 방법

- VSC에서 `F5` → 디버깅 모드 실행 (외부 터미널 사용)
- 또는 명령줄에서 직접 실행:

```bash
./main.exe
```

- 쿼리 결과는 **콘솔에 테이블 형식으로 출력**

---

## 실행 시 주의사항

- **MySQL 서버가 실행 중**이어야 한다
- `libmysql.lib` 및 헤더 경로가 **정확히 설정**되어야 한다
- `main.cpp` 내 DB 연결 정보가 **환경에 맞게 수정**되어야 한다
- `sample_data.sql`을 통해 샘플 데이터가 **정상 삽입**되어야 한다
