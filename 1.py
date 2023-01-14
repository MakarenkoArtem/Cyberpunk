from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.support.ui import Select
import time
import requests
import zipfile
import os
from selenium.webdriver.common.by import By



'''link='https://picasion.com/download/440w-all/split/'
r =requests.get(link)#, timeout=None, verify=False, allow_redirects=True)
print(r)'''


#text='https://media.giphy.com/media/KEB0JS4BdaT9m0m2vR/giphy.gif'#input()
text = input()
type_file = "http" not in text
if type_file:
    text = "\\".join(__file__.split("/")[:-1])+"\img\\"+text
x,y = map(int,input().split())






# chrome_options = Options()
chrome_options = webdriver.ChromeOptions()
prefs = {'browser.download.folderList': 2, "browser.download.dir": "\\".join(__file__.split("/")[:-1]) + "\static",
         "browser.download.manager.showWhenStarting": False}
chrome_options.add_experimental_option("prefs", prefs);
# chrome_options.add_argument("--disable-extensions")
def get_driver():
    driver = webdriver.Chrome()#chrome_options=chrome_options)
    #driver.set_preference('browser.download.folderList', 2) # custom location
    #driver.set_preference('browser.download.manager.showWhenStarting', False)
    #driver.set_preference('browser.download.dir', '/static')
    driver.implicitly_wait(30) # seconds
    return driver






def resize_file(driver, text, x, y):
    driver.get("https://www.iloveimg.com/ru/resize-image/resize-gif")
    link_but = driver.find_element(By.TAG_NAME,"input")#driver.find_element_by_xpath("//input[@type='file']")#
    link_but.send_keys(text)
    '''link_but = driver.find_element("id","pickfiles")
    link_but.click()
    link_but = driver.find_element(By.XPATH, '//label[@for="no_enlarge_if_smaller"]')#find_element("id","no_enlarge_if_smaller")
    link_but.click()'''
    link_but = driver.find_element(By.XPATH, '//label[@for="maintain_ratio"]')
    link_but.click()
    input_ = driver.find_element("id","pixels_width")
    input_.clear()
    input_.send_keys(x)
    input_ = driver.find_element("id","pixels_height")
    input_.clear()
    input_.send_keys(y)
    link_but = driver.find_element("id","processTask")
    link_but.click()
    time.sleep(5)
    input_ = driver.find_element(By.XPATH, '/html/body/div[2]/div[1]/div[1]/div[1]/a')
    for i in range(100):
        try:
            link=input_.get_attribute("href")
            break
        except Exception as e:
            print(i)
    link=input_.get_attribute("href")
    print(link)
    driver.close()
    driver.quit()
    time.sleep(5)
    for i in range(100):
        try:
            r =requests.get(link)#, timeout=None, allow_redirects=True)#, verify=False
            break
        except BaseException as e:
            print(i, e)
    open(text, 'wb').write(r.content)
    r.close()



def cut_files(driver, text, type_file):
    driver.get("https://picasion.com/ru/split-animated-gif/")
    link_but = driver.find_element("id","uploadtype_2")
    if type_file:
        link_but = driver.find_element("id","uploadtype_1")
    link_but.click()
    file_txt = driver.find_element("id","split_1_link")
    if type_file:
        file_txt = driver.find_element("id","filefoto_1_input")
    file_txt.send_keys(text)
    select = Select(driver.find_element('id','effectsel'))
    select.select_by_index(2)
    but = driver.find_element("id","sbm_button")
    but.click()
    time.sleep(5)
    link_but = driver.find_element("id","save_all")
    for i in range(100):
        print(i)
        try:
            link=link_but.get_attribute("href")
            break
        except BaseException:
            pass
    driver.close()
    driver.quit()
    print(link)
    for i in range(100):
        try:
            r =requests.get(link)#, timeout=None, verify=False, allow_redirects=True)#, verify=False
            break
        except BaseException as e:
            print(i, e)
    '''for i in range(100):
        print(i)
        try:
            r =requests.get(link, allow_redirects=True)
            break
        except BaseException:
            pass'''
    open('img/test.zip', 'wb').write(r.content)
    r.close()
    archive = zipfile.ZipFile("\\".join(__file__.split("/")[:-1])+'\\img\\test.zip', 'r') # Extract to current directory archive.extractall('.') print('ZIP Extracted.') archive.close()
    archive.extractall("\\".join(__file__.split("/")[:-1])+'\\img')
    s = {int(i.filename.split(".")[0]):i.filename for i in archive.filelist}
    archive.close()
    os.remove("\\".join(__file__.split("/")[:-1])+'\\img\\test.zip')
    st = list(s.keys())
    st.sort()
    return [s[i] for i in st]
    '''
    driver.get("https://picasion.com/ru/split-animated-gif/")
    for i in range(len(s)):
        file_txt = driver.find_element("id", "fileinput")
        file_txt.send_keys(s[i])
        file_txt = driver.find_element("id", "result")
        print(file_txt.get_attribute('value'))'''






def file_for_arduino(driver, text):
    driver.get("https://duino.ru//media/conv.html")#oled-sh1106.html/konverter-izobrazheniy/")
    link_but = driver.find_element('id', 'fileinput')
    link_but.send_keys(text)
    link_but = driver.find_element(By.XPATH, '//textarea[@id="result"]')
    return '{'+"\n".join(link_but.get_attribute("value").split("\n")[1:-1])+"}"





photos = cut_files(get_driver(), text, type_file)
photos = [ "\\".join(__file__.split("/")[:-1])+"\img\\"+ i for i in photos]
#photos=['D:\\Артём\\Arduino\\cyberpunk\\img\\0.gif', 'D:\\Артём\\Arduino\\cyberpunk\\img\\1.gif', 'D:\\Артём\\Arduino\\cyberpunk\\img\\2.gif', 'D:\\Артём\\Arduino\\cyberpunk\\img\\3.gif', 'D:\\Артём\\Arduino\\cyberpunk\\img\\4.gif', 'D:\\Артём\\Arduino\\cyberpunk\\img\\5.gif']
print(photos)
s = []
for photo in photos:
    i=1
    resize_file(get_driver(), photo, x, y)
    time.sleep(5)
    s.append(file_for_arduino(get_driver(), photo))
    #os.remove(photo)
print("{")
print(",\n".join(s))
print("};")