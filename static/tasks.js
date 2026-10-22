$(function () {
  const tasks = []; // { id, description, pomodoros, status }
  let nextId = 1;
  let editingId = null;

  const modalEl = document.getElementById("task-modal");
  const modal = new bootstrap.Modal(modalEl);
  const $list = $("#task-list");
  const $empty = $("#task-empty");
  const $desc = $("#task-description");
  const $poms = $("#task-pomodoros");
  const $error = $("#task-error");

  const ICON_EDIT =
    '<svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 20h9M16.5 3.5a2.1 2.1 0 013 3L7 19l-4 1 1-4z"/></svg>';
  const ICON_DELETE =
    '<svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 6h18M8 6V4h8v2M6 6l1 14h10l1-14M10 11v6M14 11v6"/></svg>';

  function updateEmpty() {
    $empty.toggle(tasks.length === 0);
  }

  function findTask(id) {
    return tasks.find((t) => t.id === id);
  }

  // Builds (or rebuilds) the contents of a task row
  function fillRow($li, task) {
    $li.toggleClass("done", task.status === "completed").empty();

    const $main = $('<div class="task-main"></div>');
    $main.append($('<span class="task-desc"></span>').text(task.description));

    const $meta = $('<span class="task-meta"></span>');
    $meta.append(
      '<img src="/static/assets/tomato_icon.png" alt="" width="14" height="14" />',
    );
    $meta.append(
      document.createTextNode(
        `${task.pomodoros} pomodoro${task.pomodoros === 1 ? "" : "s"}`,
      ),
    );
    $main.append($meta);

    const $select = $(
      '<select class="task-status form-select form-select-sm" aria-label="Status"></select>',
    )
      .append('<option value="pending">Pending</option>')
      .append('<option value="completed">Completed</option>')
      .val(task.status);

    const $actions = $('<div class="task-actions"></div>')
      .append($select)
      .append(
        $(
          '<button type="button" class="task-icon-btn task-edit" aria-label="Edit task"></button>',
        ).html(ICON_EDIT),
      )
      .append(
        $(
          '<button type="button" class="task-icon-btn task-delete" aria-label="Delete task"></button>',
        ).html(ICON_DELETE),
      );

    $li.append($main, $actions);
  }

  function openModal(task) {
    editingId = task ? task.id : null;
    $("#task-modal-title").text(task ? "Edit task" : "Add task");
    $desc.val(task ? task.description : "");
    $poms.val(task ? task.pomodoros : 1);
    $error.text("");
    modal.show();
  }

  function saveTask() {
    const description = $desc.val().trim();
    const pomodoros = parseInt($poms.val(), 10);

    if (!description) {
      $error.text("Please enter a description.");
      return;
    }
    if (!Number.isInteger(pomodoros) || pomodoros < 1) {
      $error.text("Pomodoros must be a whole number of at least 1.");
      return;
    }

    if (editingId !== null) {
      const task = findTask(editingId);
      task.description = description;
      task.pomodoros = pomodoros;
      fillRow($list.children(`[data-id="${task.id}"]`), task);
    } else {
      const task = { id: nextId++, description, pomodoros, status: "pending" };
      tasks.push(task);
      const $li = $('<li class="task-item task-enter"></li>').attr(
        "data-id",
        task.id,
      );
      fillRow($li, task);
      $list.append($li);
    }

    updateEmpty();
    modal.hide();
  }

  // Events
  $("#add-task-btn").on("click", () => openModal(null));
  $("#task-save").on("click", saveTask);
  $desc.add($poms).on("keydown", (e) => {
    if (e.key === "Enter") saveTask();
  });
  modalEl.addEventListener("shown.bs.modal", () => $desc.trigger("focus"));

  $list.on("change", ".task-status", function () {
    const $li = $(this).closest(".task-item");
    const task = findTask(Number($li.attr("data-id")));
    task.status = $(this).val();
    $li.toggleClass("done", task.status === "completed");
  });

  $list.on("click", ".task-edit", function () {
    const id = Number($(this).closest(".task-item").attr("data-id"));
    openModal(findTask(id));
  });

  $list.on("click", ".task-delete", function () {
    const $li = $(this).closest(".task-item");
    const id = Number($li.attr("data-id"));
    $li.addClass("task-leave");
    setTimeout(() => {
      tasks.splice(
        tasks.findIndex((t) => t.id === id),
        1,
      );
      $li.remove();
      updateEmpty();
    }, 200);
  });

  updateEmpty();
});
