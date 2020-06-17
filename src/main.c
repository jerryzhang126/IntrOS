#include <stm32f4_discovery.h>
#include <os.h>

OS_SEM(sem, 0, semBinary);

OS_TSK_DEF(cons)
{
	sem_wait(sem);
	LED_Tick();
}

OS_TSK_DEF(prod)
{
	tsk_delay(SEC);
	sem_give(sem);
}

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_sleep();
}
