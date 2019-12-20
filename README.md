# tetris_project
박제현

1. 요구사항 정의
	보드에서 플레이할 수 있는 테트리스 게임을 구현하였습니다. 게임 타이틀 화면에서 배경음악이 재생되도록 하고, 블록이 쌓일 때와 블록 한 줄이 사라질 때 효과음이 나도록 합니다. 블록은 조이스틱을 이용하여 움직이도록 하고, 블록이 화면을 넘어가면 게임이 종료되고 점수가 표시되도록 합니다.

2 .상세설계
1)문자 출력
	테트리스 게임에서는 LCD를 세로로 사용하기 때문에 LCD에 알파벳을 출력하기 위해 필요한 일부 알파벳과 숫자들을 직접 font_rotate 2차원 배열에 16진수로 표현해 한 글자 당 가로 8픽셀 * 세로 7픽셀로 표현되도록 구현하였습니다. 또한, 이 글자들을 출력하는 함수 lcd_char_rotate함수를 lcd_char 함수를 수정하여 구현하여 LCD에 한 글자씩 출력될수 있도록 구현하였습니다.

2)게임 타이틀 화면
	Hex 파일을 실행하면 게임 타이틀 화면이 나타납니다. 타이틀 화면에는 테트리스 로고와 “PRESS BTN0” 글자가 표시되며 BGM이 재생됩니다. 이 화면에서 버튼0(보드의 8개 버튼 중 제일 왼쪽 버튼)을 클릭하면 게임을 시작할 수 있습니다.

3)게임 화면
게임 화면에서는 화면 왼쪽 위에 다음 레벨업까지 클리어해야 하는 줄 수가 표시됩니다. 한 줄을 채우면 이 줄이 사라지고 왼쪽 위의 숫자가 1 줄어들며, 0이 되면 레벨업을 하게 되고 이 숫자는 다시 5가 됩니다. 블록은 화면 전체를 9칸 * 15칸으로 나누어 저장하였으며, LCD의 128 X 64 픽셀 중 105 * 63 픽셀을 사용하여 화면을 나타내었습니다.

4)테트리스 블록 구현
	테트리스 블록은 단위 블록(1 * 1칸)을 4개 사용해 테트리스 게임에서 주로 사용되는 종류들의 블록을 구현하였습니다. 이 단위 블록은 7 * 7픽셀의 크기로 구현하였습니다.

5)주요 함수 설명
A) Joystick_control 함수
	이 함수는 조이스틱을 움직인 방향을 인식해 그에 따라 떨어지는 블록을 컨트롤하는 함수입니다. 조이스틱을 좌, 우로 이동 시에는 블록이 좌, 우로 이동하고, 조이스틱을 위로 움직이면 현재 떨어지는 블록이 회전됩니다. 조이스틱을 아래로 향하면 블록이 더 빠르게 떨어지도록 구현하였습니다.

B) block0 ~ block15 함수
	이 함수는 테트리스의 블록을 화면에 표시하는 데 사용되는 함수입니다. Block0함수는 인자로 받은 x, y좌표(9 X 15 좌표)에 단위 블록을 표시하는 함수입니다. 나머지 블록들은 block0함수를 사용해 테트리스의 블록들을 생성하는 함수입니다.

C) blockadd 함수
	이 함수는 블록이 바닥에 도착하거나 다른 블록 위에 도착했을 때 쌓인 블록을 2차원 배열에 저장하는 함수입니다. 저장된 블록들은 화면에 계속 표시됩니다.

D) draw_block 함수
	이 함수는 떨어지는 블록을 LCD에 출력하는 함수입니다. 블록이 떨어질 공간이 있는지 확인하고, 만약 없다면 blockadd함수를 호출해 떨어진 블록의 위치를 2차원 배열에 저장합니다.

E) gameStart 함수
	이 함수는 게임이 시작되면 호출되는 함수로, 게임 전체를 실행시키는 함수입니다. 다음 블록을 랜덤하게 생성하고, 화면을 새로고침하며, 한 줄이 채워지면 이 줄을 지우고 점수를 획득하도록 하는 함수입니다. 블록이 바닥에 닿거나, 다른 블록 위에 닿을 때 효과음이 재생됩니다. 또한, 게임을 보다 재미있게 플레이하기 위해 4번째 블록이 떨어진 후에는 쌓여있는 블록들을 랜덤하게 좌, 우로 한 칸씩 이동시키거나 좌우 반전시킵니다. 또한, 블록이 맨 윗 칸에 닿으면 게임이 종료되도록 합니다.

F) gameTitle 함수
	게임이 시작되기 전에 나타나는 타이틀 화면을 출력하는 함수입니다. Btn0를 누르면 게임을 시작할 수 있습니다.

G) gameEnd 함수
	게임이 종료되면 점수를 출력하는 함수입니다.

3. 매뉴얼
A)타이틀 화면에서 버튼0를 누르면 게임을 시작할 수 있습니다.
B)게임 중 조이스틱을 사용해 떨어지는 블록을 좌우로 이동하고, 조이스틱을 위로 올리면 블록이 회전하고, 아래로 내리면 블록이 빠르게 떨어집니다.
C)현재 레벨은 LED로 표시되며, 레벨이 오를수록 블록이 더 빠르게 떨어집니다.
D) 다음 레벨로 레벨업하기 위해 클리어해야하는 블록 줄 수가 게임 중 왼쪽 상단에 표시됩니다.
