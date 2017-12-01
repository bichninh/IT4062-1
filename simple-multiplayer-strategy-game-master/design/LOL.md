# BTL LTM

Server nhận được opcode:

+ 0x0101: Client yêu cầu join team để chơi game
	Payload gồm: Team id.
	Server gọi đến hàm: process_join_team(team id) hàm sử lý rồi gửi kết quả trả lời cho client với:
	opcode 0x0102 payload là True or False. Kiểm tra team id có tồn tại hay ko. sl người chơi đã max chưa? Nếu chưa max thì cho vào chơi. tăng sl người chơi nhóm đó +1, return true. Ngược lại retuen false.

	
+ 0x0201: Client yêu cầu trả lời câu hỏi khai thác mỏ
def check teadm đã trả lời câu hỏi mỏ chưa? 
	Payload gồm: Mã câu hỏi mỏ ( gồm 2 byte, byte cao là mã mỏ (từ 0x01 đến 0x06), byte thấp là mã tài nguyên (0x01 đến 0x03)
	Server gọi đến hàm: send_question_mine(mã câu hỏi mỏ, loai_tai_nguyen) hàm xử lý gửi câu hỏi lại cho client với:
	opcode 0x0202 và payload (câu hỏi, đáp án[]).
	
+ 0x0301: Client gửi câu hỏi trả lời cho mỏ
	Payload gồm: Mã câu hỏi mỏ, Đáp Án (A,B,C,D)
	Server gọi đến hàm: check_answer_mine(mã câu hỏi mỏ, đáp án) kiểm tra trả lời đúng hay sai và return True or False 
gửi trả lời với	opcode 0x0302 và payload True or False

+ 0x0401: Client gửi thông điệp mua attack
	payload gồm: Mã đồ attack
	Server gọi đến hàm buy_attack(team_id, ma_attack) kiểm tra tài nguyên của team xem có đủ không.... rồi trả về kết quả:
	opcode 0x0402 payload True or False

+ 0x0501: Client gửi thông điệp tấn công lâu đài
	payload gồm: Mã lâu đài
def check 1 hàm kiểm tra lâu đài đã bị chiếm chưa
def check 2 có phải chủ lâu đài hay ko?
def check 3 có bị block hay ko?

def tấn công (nếu thành công sẽ block lâu đài)
def trả lời câu hỏi
	Server gọi đến hàm attack_castle(team_id, id_castle) kiểm tra xem lâu đài đã bị chiếm chưa:
		- nếu chưa: gửi câu trả lời với opcode: 0x0502 và payload gồm: opcode_x = 01 và payload_x = câu hỏi lâu đài và các đáp án.
		- nếu rồi: kiểm tra đồ attack của team gửi attack so sánh với defend của lâu đài:
			+ Nếu lớn hơn hoặc bằng thì block lâu đài trong 5 phút và gửi câu trả lời với opcode: 0x0502 và payload gồm: opcode_x = 02 và payload_x = câu hỏi lâu đài và các đáp án.
			+ Nếu nhỏ hơn thì gửi câu trả lời với opcode: 0x0502 và payload gồm: opcode_x = 03 và payload_x để trống

+ 0x0601: Client gửi thông điệp mua defend cho lâu đài
	payload gồm: Mã lâu đài
	Server gọi đến hàm buy_defend(team_id, id_castle) kiểm tra tài nguyên của team .... sau đó trả về kết quả:
	opcode 0x0602 và payload True or False


