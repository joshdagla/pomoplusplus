$(function () {
  const API = {
    list: { method: "GET", url: "/api/get" }, // tasks that aren't removed
    create: { method: "POST", url: "/api/create" },
    update: { method: "PUT", url: "/api/update" },
    remove: { method: "PUT", url: "/api/remove" },
  };

  let tasks = []; // { id, description, pomodoros, status }
  let editingId = null;

  const modalEl = document.getElementById("task-modal");
  const modal = new bootstrap.Modal(modalEl);
  const $list = $("#task-list");
  const $empty = $("#task-empty").hide(); // shown after the first load
  const $desc = $("#task-description");
  const $poms = $("#task-pomodoros");
  const $error = $("#task-error");
  const $save = $("#task-save");

  const ICON_EDIT =
    '<svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 20h9M16.5 3.5a2.1 2.1 0 013 3L7 19l-4 1 1-4z"/></svg>';
  const ICON_DELETE =
    '<svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 6h18M8 6V4h8v2M6 6l1 14h10l1-14M10 11v6M14 11v6"/></svg>';

  // ---------- helpers ----------
  function request(api, payload) {
    return $.ajax({
      url: api.url,
      method: api.method,
      contentType: "application/json",
      data: payload ? JSON.stringify(payload) : undefined,
      dataType: "json",
    });
  }

  function errMsg(xhr) {
    return (
      xhr.responseJSON?.message || xhr.responseText || "Something went wrong"
    );
  }

  function showError(msg) {
    alert(msg); // swap for your own UI whenever you like
  }

  function normalize(t) {
    return {
      id: Number(t.id),
      description: t.description,
      pomodoros: Number(t.pomodoros),
      status: String(t.status).toLowerCase(), // dropdown values are lowercase
    };
  }

  function updateEmpty() {
    $empty.toggle(tasks.length === 0);
  }

  function findTask(id) {
    return tasks.find((t) => t.id === id);
  }

  // ---------- rendering ----------
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

  function addRow(task) {
    const $li = $('<li class="task-item task-enter"></li>').attr(
      "data-id",
      task.id,
    );
    fillRow($li, task);
    $list.append($li);
  }

  // ---------- load ----------
  function loadTasks() {
    request(API.list)
      .done((res) => {
        tasks = (res.tasks || []).map(normalize);
        $list.empty();
        tasks.forEach(addRow);
        updateEmpty();
      })
      .fail((xhr) => showError(errMsg(xhr)));
  }

  // ---------- add / edit ----------
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

    const isEdit = editingId !== null;
    const req = isEdit
      ? request(API.update, { taskId: editingId, description, pomodoros })
      : request(API.create, { description, pomodoros });

    $save.prop("disabled", true); // stops double-clicks creating duplicates

    req
      .done((res) => {
        if (isEdit) {
          const task = findTask(editingId);
          task.description = description;
          task.pomodoros = pomodoros;
          fillRow($list.children(`[data-id="${task.id}"]`), task);
        } else {
          const task = {
            id: Number(res.id),
            description,
            pomodoros,
            status: "pending",
          };
          tasks.push(task);
          addRow(task);
        }
        updateEmpty();
        modal.hide();
      })
      .fail((xhr) => $error.text(errMsg(xhr)))
      .always(() => $save.prop("disabled", false));
  }

  // ---------- events ----------
  $("#add-task-btn").on("click", () => openModal(null));
  $save.on("click", saveTask);
  $desc.add($poms).on("keydown", (e) => {
    if (e.key === "Enter") saveTask();
  });
  modalEl.addEventListener("shown.bs.modal", () => $desc.trigger("focus"));

  $list.on("change", ".task-status", function () {
    const $select = $(this);
    const $li = $select.closest(".task-item");
    const task = findTask(Number($li.attr("data-id")));
    const newStatus = $select.val();

    request(API.update, { taskId: task.id, status: newStatus })
      .done(() => {
        task.status = newStatus;
        $li.toggleClass("done", newStatus === "completed");
      })
      .fail((xhr) => {
        $select.val(task.status); // put the dropdown back
        showError(errMsg(xhr));
      });
  });

  $list.on("click", ".task-edit", function () {
    const id = Number($(this).closest(".task-item").attr("data-id"));
    openModal(findTask(id));
  });

  // "Delete" only marks the task as removed on the server
  $list.on("click", ".task-delete", function () {
    const $btn = $(this);
    const $li = $btn.closest(".task-item");
    const id = Number($li.attr("data-id"));

    $btn.prop("disabled", true);

    request(API.remove, { taskId: id })
      .done(() => {
        $li.addClass("task-leave");
        setTimeout(() => {
          tasks = tasks.filter((t) => t.id !== id);
          $li.remove();
          updateEmpty();
        }, 200);
      })
      .fail((xhr) => {
        $btn.prop("disabled", false);
        showError(errMsg(xhr));
      });
  });

  loadTasks();
});
