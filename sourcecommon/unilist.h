//---------------------------------------------------------------------------
#ifndef unilistH
#define unilistH

/*
  Шаблон TUniList: список указателей на объекты
  Создание, удаление и копирование объектов типа T не выполняется !
  Подходит для организации стека объектов: Add() / ExtractLast()
  или очереди объектов: Add() / Extract(0)

  Есть возможность включения межпоточной блокировки для всех методов,
  кроме ItemsNB[], CountNB
  Для включения межпоточной блокировки надо вызвать EnableMultiTreading()
  один раз после создания объекта.

  Примеры использования:
  1) Без блокировки:
    class c { int i; };
    TUniList<c> cl; // список указателей на c

    cl.Add( new c() );

    c * p = cl.ExtractLast();
    if( ! p )
      ; // список cl пуст
    else
      // p - содержит указатель на последний объект в списке,
      // список уменьшился на 1
      delete p;

    // Удаляем все объекты в списке
    for(int i=0, l=cl.Count; i<l; i++)
      delete (c *)cl.Items[i];
    cl.Clear(); // Очищаем список

  2) Используется межпоточная блокировка:
    class c { int i; };
    TUniList<c> cl; // список указателей на c
    cl.EnableMultiTreading();

    // Add() и Extract() можно вызывать из разных потоков
    cl.Add( new c() );

    c * p = cl.Extract(0);
    if( ! p )
      ; // список cl пуст
    else
      // p - содержит указатель по индексу 0, а список уменьшился на 1
      delete p;

    // Пример прохода по списку объектов с управляемой блокировкой
    // Удаляем все объекты в списке
    cl.SetBusy(true);
    try
    {
      for(int i=0, l=cl.CountNB; i<l; i++)
        delete (c *)cl.ItemsNB[i];
      cl.Clear(); // Очищаем список
    }
    __finally
    { cl.SetBusy(false); }

  Автор: Бельков М.
  Версия 2

  Внимание: unilist.cpp включать в проект нельзя !
  #include "unilist.h" // и все
*/

#include <vcl.h>

#include "critsection.h"

//---------------------------------------------------------------------------
template <class T>
class TUniList
{
private:
  TList * l;           // vcl TList
  TCritSection * pcs;  // Critical section for multi-thread access

public:
  TUniList(void);
  ~TUniList(void);

  // Start muti-thread syncronization in all methods except ItemsNB[], CountNB
  void EnableMultiTreading(void);

  // Очистка списка
  // Удаление объектов не выполняется
  void Clear(void);
  // Добавляет указатель p в конец списка.
  // Копирование объекта не производится.
  // Возвращает индекс нового указателя в списке (с нуля)
  int Add(T * p);
  // Вставляет указатель p в позицию i (с нуля) списка.
  // Копирование объекта не производится.
  void Insert(int i, T * p);
  // Удаляет указатель p из списка
  void Delete(T * p);
  // Удаляет указатель с индексом i из списка
  // Возвращает указатель на удаленный объект или NULL
  T * Extract(int i);
  // То-же для последнего элемента списка
  T * ExtractLast(void);
  // Проверяет наличие указателя p в списке
  bool Contains(T * p);
  // Доступ к указателю объекта по его индексу
  __property T * Items[int i] = {read=Get, write=Set};
  // Количество объектов в списке
  __property int Count = {read=GetCount};

  // То-же, что Items[], но без блокировки
  __property T * ItemsNB[int i] = {read=GetNB, write=SetNB};
  // То-же, что Count, но без блокировки
  __property int CountNB = {read=GetCountNB};
  // Включение/выключение блокировки
  bool SetBusy(bool b);

private:
  T * Get(int i);
  void Set(int i, T * p);
  int GetCount(void);

  T * GetNB(int i);
  void SetNB(int i, T * p);
  int GetCountNB(void);
};
//---------------------------------------------------------------------------

#include "unilist.cpp"

#endif
