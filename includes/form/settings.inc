<?php

/**
 * @file
 * Contains the settings form callbacks.
 *
 * @author Jim Berry ("solotandem", http://drupal.org/user/240748)
 */

/**
 * Form constructor for the container form.
 *
 * @see google_tag_settings_form_validate()
 * @see google_tag_settings_form_submit()
 *
 * @ingroup forms
 */
function google_tag_settings_form($form, &$form_state) {
  module_load_include('inc', 'google_tag', 'includes/variable');
//   module_load_include('inc', 'ctools', 'includes/export');

  // Gather data.
  // @todo Set this on validation errors.
  $default_tab = !empty($form_state['default_tab']) ? $form_state['default_tab'] : '';
  // @todo always add default keys in case of no update hook being run.
  $config = \GTMSettings::getInstance();

  $export_type = EXPORT_IN_DATABASE;
  if (empty($config->get('uri'))) {
    // @todo Remove the 'general' keys. This call now omits the settings keys.
    $config = (object) GTMContainerExport::variables_get();
    $export_type = NULL;
  }
  $form_state['item'] = $config;

  // Build form elements.

  $description = t('<br />After configuring the module settings and default container settings, <strong>add a container</strong> on the <a href="!url">container management page</a>.', array('!url' => url('admin/config/system/google_tag')));

  $form['instruction'] = array(
    '#type' => 'markup',
    '#markup' => $description,
  );

  // Module settings.
  $groups = array(
    'settings' => array('title' => t('Module settings'), 'description' => t('The settings that apply to all containers.'), 'collapse' => FALSE),
  );

  foreach ($groups as $group => $items) {
    $form['settings'] = google_tag_fieldset($group, $items, $form_state['item']);
  }

  // Default container settings.
  $description = t('On this and the next two tabs, specify the conditions on which the GTM JavaScript snippet will either be included in or excluded from the page response, thereby enabling or disabling tracking and other analytics. All conditions must be satisfied for the snippet to be included. The snippet will be excluded if any condition is not met.<br /><br />On this tab, specify the path condition.');
  $text = t('The default container settings that apply to a new container.');
  $suffix = '<div id="edit-default--description" class="description">' . $text . '</div>';

  $groups = array(
//     'general' => array('title' => t('General'), 'collapse' => FALSE),
    'path' => array('title' => t('Page paths'), 'description' => $description),
    'role' => array('title' => t('User roles'), 'description' => t('On this tab, specify the user role condition.')),
    'status' => array('title' => t('Response statuses'), 'description' => t('On this tab, specify the page response status condition.')),
    'advanced' => array('title' => t('Advanced')),
  );

  // Use prefix and suffix as title and description are not rendered.
  $form['tabs'] = array(
    '#type' => 'vertical_tabs',
    '#title' => t('Default container settings'),
    '#description' => t('The default container settings that apply to a new container.'),
    '#default_tab' => $default_tab ? $default_tab : 'edit-general',
    '#attributes' => array('class' => array('google-tag')),
    '#attached' => array(
      'css' => array(drupal_get_path('module', 'google_tag') . '/css/google_tag.admin.css'),
      'js' => array(drupal_get_path('module', 'google_tag') . '/js/google_tag.admin.js'),
    ),
    '#prefix' => '<strong>' . t('Default container settings') . '</strong>',
    '#suffix' => $suffix,
  );

  foreach ($groups as $group => $items) {
    $form['tabs'][$group] = google_tag_fieldset($group, $items, $form_state['item']);
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  $form['export_type'] = array(
    '#type' => 'value',
    '#value' => $export_type,
  );

  return $form;
}

/**
 * Fieldset builder for the module settings form.
 */
function google_tag_fieldset($group, array $items, $container) {
  // Gather data.
  $function = "_google_tag_variable_info_$group";
  $variables = $function(array());
  $items += array('description' => '', 'collapse' => TRUE);

  // Build form elements.
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => $items['title'],
    '#description' => $items['description'],
    '#collapsible' => $items['collapse'],
    '#collapsed' => $items['collapse'],
    '#tree' => FALSE,
  );
  $fieldset += google_tag_form_elements($variables, $container);

  return $fieldset;
}

/**
 * Returns form elements from variable definitions.
 *
 * @param array $variables
 *   Associative array of variable definitions.
 * @param object $container
 *   The container configuration object.
 *
 * @return array
 *   Associative array of form elements.
 */
function google_tag_form_elements(array $variables, $container) {
  static $keys = array(
    'type' => '#type',
    'title' => '#title',
    'description' => '#description',
    'options' => '#options',
    'default' => '#default_value',
  );
  $elements = array();
  foreach ($variables as $name => $variable) {
    $element = array();
    foreach ($keys as $key => $property) {
      if (isset($variable[$key])) {
        $element[$property] = $variable[$key];
      }
    }
    $key = str_replace('google_tag_', '', $name);
    $element['#type'] = google_tag_form_element_type($element['#type']);
    $element['#default_value'] = $container->$key;
    $element += isset($variable['element']) ? $variable['element'] : array();
    $elements[$name] = $element;
  }

  return $elements;
}

/**
 * Converts variable type to form element type.
 *
 * @param string $type
 *   Variable type.
 *
 * @return string
 *   Form element type.
 */
function google_tag_form_element_type($type) {
  static $keys = array(
    'string' => 'textfield',
    'select' => 'radios',
    'text' => 'textarea',
    'options' => 'checkboxes',
    'boolean' => 'checkbox',
    'weight' => 'weight',
  );
  return isset($keys[$type]) ? $keys[$type] : 'textfield';
}

/**
 * Form validation handler for google_tag_settings_form().
 */
function google_tag_settings_form_validate($form, &$form_state) {
  $values = &$form_state['values'];

  // Trim the text values.
//   $values['google_tag_container_id'] = trim($values['google_tag_container_id']);
  $values['google_tag_uri'] = trim($values['google_tag_uri']);
  $values['google_tag_environment_id'] = trim($values['google_tag_environment_id']);
  $values['google_tag_data_layer'] = trim($values['google_tag_data_layer']);
  google_tag_text_clean($values['google_tag_path_list']);
  google_tag_text_clean($values['google_tag_status_list']);
  google_tag_text_clean($values['google_tag_whitelist_classes']);
  google_tag_text_clean($values['google_tag_blacklist_classes']);

  // Replace all types of dashes (n-dash, m-dash, minus) with a normal dash.
//   $values['google_tag_container_id'] = str_replace(array('–', '—', '−'), '-', $values['google_tag_container_id']);
  $values['google_tag_environment_id'] = str_replace(array('–', '—', '−'), '-', $values['google_tag_environment_id']);
/*
  if (!preg_match('/^GTM-\w{4,}$/', $values['google_tag_container_id'])) {
    // @todo Is there a more specific regular expression that applies?
    // @todo Is there a way to "test the connection" to determine a valid ID for
    // a container? It may be valid but not the correct one for the website.
    form_set_error('google_tag_container_id', t('A valid container ID is case sensitive and formatted like GTM-xxxxxx.'));
  }
*/
  if ($values['google_tag_include_environment'] && !preg_match('/^env-\d{1,}$/', $values['google_tag_environment_id'])) {
    form_set_error('google_tag_environment_id', t('A valid environment ID is case sensitive and formatted like env-x.'));
  }
  if ($message = _google_tag_data_layer_verify($values['google_tag_data_layer'])) {
    form_set_error('google_tag_data_layer', $message);
  }
  if ($values['google_tag_include_classes']) {
    if (empty($values['google_tag_whitelist_classes']) && empty($values['google_tag_blacklist_classes'])) {
      form_set_error('google_tag_include_classes', t('Enter listed classes in at least one field, or uncheck the box.'));
      form_set_error('google_tag_whitelist_classes', '');
      form_set_error('google_tag_blacklist_classes', '');
    }
  }

  $directory = $values['google_tag_uri'];
  if (substr($directory, -3) == '://') {
    $args = array('%directory' => $directory);
    $message = 'The snippet parent uri %directory is invalid. Enter a single trailing slash to specify a plain stream wrapper.';
    form_set_error('google_tag_uri', t($message, $args));
  }

  // Allow for a plain stream wrapper with one trailing slash.
  $directory .= substr($directory, -2) == ':/' ? '/' : '';
  if (!is_dir($directory) || !_google_tag_is_writable($directory) || !_google_tag_is_executable($directory)) {
    $args = array('%directory' => $directory);
    $message = 'The snippet parent uri %directory is invalid, possibly due to file system permissions. The directory either does not exist, or is not writable or searchable.';
    form_set_error('google_tag_uri', t($message, $args));
  }
}

/**
 * Form submission handler for google_tag_settings_form().
 */
function google_tag_settings_form_submit($form, &$form_state) {
  module_load_include('inc', 'ctools', 'includes/export');

  // @todo Just like fields that change the 'value' during load-save,
  // backup the current form_state 'item' and restore it after save.
  // @todo Add the status property to data.
  $variables = GTMContainerExport::variables_get();
  // Set name and label so we can load all rows in config table and then remove
  // the settings item before building the container list page.
  // See google_tag_export_load().
  $values = &$form_state['values'];
  $data['name'] = 'settings';
  $data['label'] = 'Module settings';
  // @todo ctools seems to store this in the variable table for all objects???
  // It relies on config.export.status key
  $data['status'] = TRUE;
  $data += array_intersect_key($form_state['values'], $variables);
  $clone['name'] = 'google_tag.settings';
  $clone['data'] = $data;
  $clone['export_type'] = isset($values['export_type']) ? $values['export_type'] : NULL;
  $clone = (object) $clone;

  $result = ctools_export_crud_save('gtag_config', $clone);
  // @todo If the uri changes, then could rebuild snippets. But let user invoke.
}

// Next two functions are in two files: this and form/container.inc
// For code in php cache does this cause issue? It works on php 7.3.

/**
 * Cleans a string representing a list of items.
 *
 * @param string $text
 *   The string to clean.
 * @param string $format
 *   The final format of $text, either 'string' or 'array'.
 */
function google_tag_text_clean(&$text, $format = 'string') {
  $text = explode("\n", $text);
  $text = array_map('trim', $text);
  $text = array_filter($text, 'trim');
  if ($format == 'string') {
    $text = implode("\n", $text);
  }
}

/**
 * Verifies presence of dataLayer module and compares name of data layer.
 *
 * @return bool|null
 *   Whether data layer name is incompatible with dataLayer module, if present.
 */
function _google_tag_data_layer_verify($value) {
  if (module_exists('datalayer') && $value != 'dataLayer') {
    // @todo Setting form error does not allow user to save a different name,
    // i.e. to do what message text says.
    return t('The 1.1 release of the dataLayer module does not support a data layer name other than "dataLayer." If you need a different layer name, then either disable the dataLayer module or alter the JavaScript added to the page response.');
  }
}
