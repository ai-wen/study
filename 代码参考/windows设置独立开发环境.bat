@echo off

::: �ѡ�G:\����Ŀ�з�\OTP���ļ��������A������
subst L: "G:\����Ŀ�з�\OTP"

L:

:::  �ѵ�ǰ·�����뵽 DEVELOPMENT  �Ļ���������
setx MYDEVELOPMENT   %cd%	

::: ���ӳ��
:::  wmic ENVIRONMENT where "name='MYDEVELOPMENT'" delete

::: ���ӳ��
::: subst /D L: 

pause