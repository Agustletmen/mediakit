
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


int main()
{
    // ================================= 1. ������ͷ =================================
    // VideoCapture(0)����Ĭ������ͷ���ʼǱ���������ͷͨ���� 0��
    // ���ж������ͷ���ɳ��� 1��2 �ȣ����������ͷ������ 1��
    VideoCapture cap(0);

    // �������ͷ�Ƿ�򿪳ɹ�
    if (!cap.isOpened())
    {
        cout << "? �����޷�������ͷ��" << endl;
        cout << "����ԭ��" << endl;
        cout << "1. ����ͷ����������ռ�ã���΢�š�Zoom��" << endl;
        cout << "2. �޿�������ͷ�����������ͷδ���ӣ�" << endl;
        cout << "3. Ȩ�޲��㣨Windows ������Ӧ�÷�������ͷ��" << endl;
        return -1;
    }

    // ================================= 2. ��������ͷ��������ѡ�� =================================
    // ������Ƶ֡���ȣ���������ͷ֧�ֵķֱ��ʵ������� 1920x1080��1280x720��
    cap.set(CAP_PROP_FRAME_WIDTH, 1280);
    // ������Ƶ֡�߶�
    cap.set(CAP_PROP_FRAME_HEIGHT, 720);
    // ����֡�ʣ���������ͷ���ܲ�֧���Զ��壬Ĭ��ͨ���� 30fps��
    cap.set(CAP_PROP_FPS, 30);

    // ��ӡ����ͷʵ�ʲ�������֤�����Ƿ���Ч��
    cout << "? ����ͷ�򿪳ɹ���" << endl;
    cout << "�ֱ��ʣ�" << cap.get(CAP_PROP_FRAME_WIDTH) << "x" << cap.get(CAP_PROP_FRAME_HEIGHT) << endl;
    cout << "֡�ʣ�" << cap.get(CAP_PROP_FPS) << "fps" << endl;
    cout << "����˵����" << endl;
    cout << "  - �� ESC �����˳�����" << endl;
    cout << "  - �� S �������浱ǰ֡Ϊ��ͼ��screenshot.jpg��" << endl;

    // ================================= 3. ѭ����ȡ����ʾ��Ƶ�� =================================
    Mat frame; // �洢ÿ֡ͼ��ľ���
    namedWindow("����ͷʵʱ����", WINDOW_NORMAL); // �ɵ������ڴ�С

    while (true)
    {
        // ��ȡһ֡ͼ��cap >> frame �ȼ��� cap.read(frame)��
        cap.read(frame);

        // ���֡�Ƿ��ȡ�ɹ�������ͷ�Ͽ�ʱ��ʧ�ܣ�
        if (frame.empty())
        {
            cout << "??  ���棺�޷���ȡ����ͷ���棬�����ѶϿ����ӣ�" << endl;
            break;
        }

        // �ڻ����ϻ���������ʾ����ѡ����ǿ�����ԣ�
        putText(
            frame, // Ҫ���Ƶ�ͼ��
            "ESC: Quit | S: Screenshot", // ��������
            Point(20, 40), // ����λ�ã����Ͻǣ�
            FONT_HERSHEY_SIMPLEX, // ����
            1.0, // �����С
            Scalar(0, 255, 0), // ������ɫ��BGR����ɫ��
            2 // ���ֺ��
        );

        // ��ʾ��ǰ֡
        imshow("����ͷʵʱ����", frame);

        // �������ƣ��ȴ� 1ms�����⻭�濨�٣�
        int key = waitKey(1);
        if (key == 27)
        {
            // ESC ����ASCII �� 27�����˳�����
            cout << "? �յ��˳�ָ����򼴽��ر�..." << endl;
            break;
        }
        else if (key == 's' || key == 'S')
        {
            // S ���������ͼ
            imwrite("screenshot.jpg", frame);
            cout << "? ��ͼ�ѱ���Ϊ screenshot.jpg�����Ŀ¼�£�" << endl;
        }
    }

    // ================================= 4. �ͷ���Դ =================================
    cap.release(); // �ر�����ͷ
    destroyAllWindows(); // �ر����д���
    cout << "? ���������˳���" << endl;

    return 0;
}
