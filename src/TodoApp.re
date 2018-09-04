/* Application state */
type state = {
  new_list_name: string,
  lists: array(TodoList.todo_list),
};

/* Action declaration */
type action =
  | AddList(string)
  | ChangeNewListName(string)
  | DragItem(string, string, string);

/* Component template declaration.
   Needs to be **after** state and action declarations! */
let component = ReasonReact.reducerComponent("TodoApp");

let make = (_children) => {
  /* spread the other default fields of component here and override a few */
  ...component,

  initialState: () => {
    /* lists: [||], */
    lists: TodoList.demo_lists(2),
    new_list_name: "",
  },

  /* State transitions */
  reducer: (action, state) =>
    switch (action) {
    | AddList(name) => ReasonReact.Update({
        ...state,
        lists: Array.append(
          state.lists,
          [| TodoList.empty_list(name) |]
        ),
        new_list_name: ""
      })
    | ChangeNewListName(text) => ReasonReact.Update({...state, new_list_name: text})
    | DragItem(src_list_id, src_item_id, dest_list_id) => {
        Js.log(("drag item, src_list_id: ", src_list_id, "src_item_id", src_item_id, "dest_list_id", dest_list_id));
        /* TODO: this shouldn't be necessary but for some reason I'm getting the
           DragItem event fired twice.
           What's interesting -- this doesn't happen for demo_lists, demo_items.
           Is this some double binding set up on sortable? */
        let new_lists = try
          (TodoList.move_item(state.lists, src_list_id, src_item_id, dest_list_id)) {
          | Not_found => state.lists
        };
        ReasonReact.Update({
          ...state,
          lists: new_lists
        })
      }
    },

  render: self => {
    let todoLists = (lsts) =>
      Array.mapi
      (
      (index, lst: TodoList.todo_list) =>
        <TodoList lst
            key=lst.id
            onDragItem=((src_list_id, src_item_id, dest_list_id) => self.send(DragItem(src_list_id, src_item_id, dest_list_id))) />,
        lsts
    );

    <div className="container">
      <h1 className="title">(ReasonReact.string("Todo Lists"))</h1>
      <div className="level">(ReasonReact.array(todoLists(self.state.lists)))
        <div className="level-item">
          <div className="field">
            <div className="control">
              <input className="input is-primary"
                  value=self.state.new_list_name
                  onChange=(
                    event =>
                      self.send(ChangeNewListName(ReactEvent.Form.target(event)##value))
                  )/>
              <a className="button" onClick=(_event => self.send(AddList(self.state.new_list_name)))>
                  (ReasonReact.string("Add list"))
              </a>
            </div>
          </div>
        </div>
      </div>
    </div>;
  }
};
