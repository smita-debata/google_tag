<?php

/**
 * @file
 * Contains the administrative page and form callbacks.
 *
 * @author Jim Berry ("solotandem", http://drupal.org/user/240748)
 */

/**
 * Form constructor for the module settings form.
 *
 * @see google_tag_settings_form_validate()
 *
 * @ingroup forms
 */
function google_tag_settings_form($form, &$form_state) {
  module_load_include('inc', 'google_tag', 'includes/variable');

  // Gather data.
  // @todo Set this on validation errors.
  $default_tab = !empty($form_state['default_tab']) ? $form_state['default_tab'] : '';

  // Build form elements.
  $form['tabs'] = array(
    '#type' => 'vertical_tabs',
    '#default_tab' => $default_tab ? $default_tab : 'edit-general',
    '#attributes' => array('class' => array('google-tag')),
    '#attached' => array(
      'css' => array(drupal_get_path('module', 'google_tag') . '/css/google_tag.admin.css'),
      'js' => array(drupal_get_path('module', 'google_tag') . '/js/google_tag.admin.js'),
    ),
  );
  $form['tabs']['general'] = google_tag_general_fieldset($form_state);
  $form['tabs']['paths'] = google_tag_path_fieldset($form_state);
  $form['tabs']['roles'] = google_tag_role_fieldset($form_state);
  $form['tabs']['statuses'] = google_tag_status_fieldset($form_state);
  $form['tabs']['advanced'] = google_tag_advanced_fieldset($form_state);

  return system_settings_form($form);
}

/**
 * Fieldset builder for the module settings form.
 */
function google_tag_general_fieldset(&$form_state) {
  // Gather data.
  $variables = _google_tag_variable_info_general([]);

  // Build form elements.
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('General'),
    '#tree' => FALSE,
  );
  $fieldset += google_tag_form_elements($variables);

  return $fieldset;
}

/**
 * Fieldset builder for the module settings form.
 *
 * Allows user to specify snippet insertion condition based on page path.
 */
function google_tag_path_fieldset(&$form_state) {
  // Gather data.
  $variables = _google_tag_variable_info_path([]);

  $description = t('On this and the next two tabs, specify the conditions on which the GTM JavaScript snippet will either be included in or excluded from the page response, thereby enabling or disabling tracking and other analytics.');
  $description .= t(' All conditions must be satisfied for the snippet to be included. The snippet will be excluded if any condition is not met.<br /><br />');
  $description .= t(' On this tab, specify the path condition.');

  // Build form elements.
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Page paths'),
    '#description' => $description,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => FALSE,
  );
  $fieldset += google_tag_form_elements($variables);

  return $fieldset;
}

/**
 * Fieldset builder for the module settings form.
 *
 * Allows user to specify snippet insertion condition based on user role.
 */
function google_tag_role_fieldset(&$form_state) {
  // Gather data.
  $variables = _google_tag_variable_info_role([]);

  // Build form elements.
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('User roles'),
    '#description' => t('On this tab, specify the user role condition.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => FALSE,
  );
  $fieldset += google_tag_form_elements($variables);

  return $fieldset;
}

/**
 * Fieldset builder for the module settings form.
 *
 * Allows user to specify snippet insertion condition based on response status.
 */
function google_tag_status_fieldset(&$form_state) {
  // Gather data.
  $variables = _google_tag_variable_info_status([]);

  // Build form elements.
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Response statuses'),
    '#description' => t('On this tab, specify the page response status condition.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => FALSE,
  );
  $fieldset += google_tag_form_elements($variables);

  return $fieldset;
}

/**
 * Fieldset builder for the module settings form.
 */
function google_tag_advanced_fieldset(&$form_state) {
  // Gather data.
  $variables = _google_tag_variable_info_advanced([]);

  // Build form elements.
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => FALSE,
  );
  $fieldset += google_tag_form_elements($variables);

  return $fieldset;
}

/**
 * Returns form elements from variable definitions.
 *
 * @param array $variables
 *   Associative array of variable definitions.
 *
 * @return array
 *   Associative array of form elements.
 */
function google_tag_form_elements($variables) {
  static $keys = [
    'type' => '#type',
    'title' => '#title',
    'description' => '#description',
    'options' => '#options',
    'default' => '#default_value',
  ];
  $elements = [];
  foreach ($variables as $name => $variable) {
    $element = [];
    foreach ($keys as $key => $property) {
      if (isset($variable[$key])) {
        $element[$property] = $variable[$key];
      }
    }
    $element['#type'] = google_tag_form_element_type($element['#type']);
    $element['#default_value'] = variable_get($name, $variable['default']);
    $element += isset($variable['element']) ? $variable['element'] : [];
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
  static $keys = [
    'string' => 'textfield',
    'select' => 'radios',
    'text' => 'textarea',
    'options' => 'checkboxes',
    'boolean' => 'checkbox',
  ];
  return isset($keys[$type]) ? $keys[$type] : 'textfield';
}

/**
 * Form validation handler for google_tag_settings_form().
 */
function google_tag_settings_form_validate($form, &$form_state) {
  $values = &$form_state['values'];

  // Trim the text values.
  $values['google_tag_container_id'] = trim($values['google_tag_container_id']);
  $values['google_tag_path_list'] = trim($values['google_tag_path_list']);
  $values['google_tag_status_list'] = trim($values['google_tag_status_list']);

  // Replace all types of dashes (n-dash, m-dash, minus) with a normal dash.
  $values['google_tag_container_id'] = str_replace(array('–', '—', '−'), '-', $values['google_tag_container_id']);

  if (!preg_match('/^GTM-\w{4,}$/', $values['google_tag_container_id'])) {
    // @todo Is there a more specific regular expression that applies?
    // @todo Is there a way to "test the connection" to determine a valid ID for
    // a container? It may be valid but not the correct one for the website.
    form_set_error('google_tag_container_id', t('A valid container ID is case sensitive and formatted like GTM-xxxxxx.'));
  }
}
