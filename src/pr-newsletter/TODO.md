Code Generator for adding a new state and page
- Ask for state name
- Verify if name is unique
- Add folder app/_states/_${name}
- Add page file app/_states/_${name}/${upperCase(name)}Page.tsx
  * with usual data structures for state and it's page
- Add instance of the new page in SinglePage.tsx before the comment "add new pages here"
- create folder app/[lang]/${name}
- create app/[lang]/${name}/page.tsx similar to app/[lang]/mdEdit/[newsletterId]/[md]/[mdId]/page.tsx

workbench.action.quickOpen