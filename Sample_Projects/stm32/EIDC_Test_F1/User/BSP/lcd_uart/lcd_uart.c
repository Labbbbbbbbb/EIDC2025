
#include "lcd_uart.h"



#if USART_EN_RX /*如果使能了接收*/

/* 接收缓冲, 最大USART_REC_LEN个字节. */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/*  接收状态
 *  bit15，      接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
*/
uint16_t g_usart_rx_sta = 0;

uint8_t g_rx_buffer[RXBUFFERSIZE];  /* HAL库使用的串口接收缓冲 */

UART_HandleTypeDef g_uart3_handle;  /* UART句柄 */

/**
 * @brief       串口X初始化函数
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @note        注意: 必须设置正确的时钟源, 否则串口波特率就会设置异常.
 *              这里的USART的时钟源在sys_stm32_clock_init()函数中已经设置过了.
 * @retval      无
 */
void usart_init(uint32_t baudrate)
{
    /*UART 初始化设置*/
    g_uart3_handle.Instance = USART_UX;                                       /* USART_UX */
    g_uart3_handle.Init.BaudRate = baudrate;                                  /* 波特率 */
    g_uart3_handle.Init.WordLength = UART_WORDLENGTH_8B;                      /* 字长为8位数据格式 */
    g_uart3_handle.Init.StopBits = UART_STOPBITS_1;                           /* 一个停止位 */
    g_uart3_handle.Init.Parity = UART_PARITY_NONE;                            /* 无奇偶校验位 */
    g_uart3_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;                      /* 无硬件流控 */
    g_uart3_handle.Init.Mode = UART_MODE_TX_RX;                               /* 收发模式 */
    HAL_UART_Init(&g_uart3_handle);                                           /* HAL_UART_Init()会使能UART1 */

    /* 该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量 */
    HAL_UART_Receive_IT(&g_uart3_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE); 
}

/**
 * @brief       UART底层初始化函数
 * @param       huart: UART句柄类型指针
 * @note        此函数会被HAL_UART_Init()调用
 *              完成时钟使能，引脚配置，中断配置
 * @retval      无
 */
// void HAL_UART_MspInit(UART_HandleTypeDef *huart)
// {
//     GPIO_InitTypeDef gpio_init_struct;

//     if (huart->Instance == USART_UX)                            /* 如果是串口3，进行串口3 MSP初始化 */
//     {
//         USART_TX_GPIO_CLK_ENABLE();                             /* 使能串口TX脚时钟 */
//         USART_RX_GPIO_CLK_ENABLE();                             /* 使能串口RX脚时钟 */
//         USART_UX_CLK_ENABLE();                                  /* 使能串口时钟 */

//         gpio_init_struct.Pin = USART_TX_GPIO_PIN;               /* 串口发送引脚号 */
//         gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 复用推挽输出 */
//         gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
//         gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* IO速度设置为高速 */
//         HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct);
                
//         gpio_init_struct.Pin = USART_RX_GPIO_PIN;               /* 串口RX脚 模式设置 */
//         gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;    
//         HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct);   /* 串口RX脚 必须设置成输入模式 */
        
// #if USART_EN_RX
//         HAL_NVIC_EnableIRQ(USART_UX_IRQn);                      /* 使能USART1中断通道 */
//         HAL_NVIC_SetPriority(USART_UX_IRQn, 3, 3);              /* 组2，最低优先级:抢占优先级3，子优先级3 */
// #endif
//     }
// }

/**
 * @brief       串口数据接收回调函数
                数据处理在这里进行
 * @param       huart:串口句柄
 * @retval      无
 */
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == USART_UX)                    /* 如果是串口1 */
//     {
//         if ((g_usart_rx_sta & 0x8000) == 0)             /* 接收未完成 */
//         {
//             if (g_usart_rx_sta & 0x4000)                /* 接收到了0x0d（即回车键） */
//             {
//                 if (g_rx_buffer[0] != 0x0a)             /* 接收到的不是0x0a（即不是换行键） */
//                 {
//                     g_usart_rx_sta = 0;                 /* 接收错误,重新开始 */
//                 }
//                 else                                    /* 接收到的是0x0a（即换行键） */
//                 {
//                     g_usart_rx_sta |= 0x8000;           /* 接收完成了 */
//                 }
//             }
//             else                                        /* 还没收到0X0d（即回车键） */
//             {
//                 if (g_rx_buffer[0] == 0x0d)
//                     g_usart_rx_sta |= 0x4000;
//                 else
//                 {
//                     g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = g_rx_buffer[0];
//                     g_usart_rx_sta++;

//                     if (g_usart_rx_sta > (USART_REC_LEN - 1))
//                     {
//                         g_usart_rx_sta = 0;             /* 接收数据错误,重新开始接收 */
//                     }
//                 }
//             }
//         }
//     }
// }

// /**
//  * @brief       串口X中断服务函数
//                 注意,读取USARTx->SR能避免莫名其妙的错误
//  * @param       无
//  * @retval      无
//  */
// void USART_UX_IRQHandler(void)
// {
// #if SYS_SUPPORT_OS                                                   /* 使用OS */
//     OSIntEnter();
// #endif
//     HAL_UART_IRQHandler(&g_uart3_handle);                               /* 调用HAL库中断处理公用函数 */

//     while (HAL_UART_Receive_IT(&g_uart3_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE) != HAL_OK)     /* 重新开启中断并接收数据 */
//     {
//         /* 如果出错会卡死在这里 */
//     }

// #if SYS_SUPPORT_OS                                                   /* 使用OS */
//     OSIntExit();
// #endif
// }
 #endif
