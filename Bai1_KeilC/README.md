# 🔦 Blink LED - STM32F103C8 (PC13)

Đây là bài tập đầu tiên trong khóa học lập trình vi điều khiển STM32: **Nháy LED đơn tại chân PC13** sử dụng board **STM32F103C8T6 (Blue Pill)**.

## 🛠️ Mô tả

- Bật/tắt LED nối với chân PC13 liên tục theo chu kỳ.
- Sử dụng truy cập thanh ghi trực tiếp (bare-metal C).


## 🧠 Kiến thức áp dụng

- Bật clock cho GPIO (RCC_APB2ENR).
- Cấu hình chân PC13 output mode (GPIOC_CRH).
- Ghi ra thanh ghi ODR để điều khiển LED.
- Dùng `volatile` để tránh tối ưu hóa khi delay.

## 🖥️ Cấu hình phần cứng

- Vi điều khiển: STM32F103C8T6
- ST-Link V2 để nạp chương trình

